#include "Utilities/calibrationRig.h"

#include <boost/filesystem/path.hpp>
#include <boost/format.hpp>

#include "Base/JsonSerializer.h"
#include "Utilities/glmHelpers.h"

#include <fstream>
#include <glm/gtx/euler_angles.hpp>

namespace Limitless
{

CalibrationRig::CalibrationRig():
	m_loaded(false)
{
	m_name="unknown";
}

CalibrationRig::~CalibrationRig()
{}

bool CalibrationRig::load(std::string location, std::string *error)
{
	FILE *file=fopen(location.c_str(), "r");

	if(file==NULL)
	{
		if(error!=nullptr)
		{
			(*error)="Unable to locate file "+location;
		}
		return false;
	}

	std::string line(30, (char)0);
	fread((char *)line.data(), sizeof(char), 20, file);
	fclose(file);

	if(line.find("# hugin project file")!=std::string::npos)
	{
		openHuginCalib(location);
		return true;
	}

	Limitless::JsonUnserializer unserializer;

	if(unserializer.open(location))
	{
		std::string localError;

		if(!loadCameraParams(unserializer, &localError))
		{
			return false;
		}
	}

	boost::filesystem::path filePath(location);

	m_name=filePath.filename().string();
	m_location=location;

	m_loaded=true;
}


bool CalibrationRig::loadCameraParams(Unserializer &unserializer, std::string *error)
{
	bool requiredItemMissing=false;
	std::vector<std::string> messages;

	if(unserializer.type()!=Type::OBJECT)
	{
		messages.push_back("Expected object at root of file");
		return false;
	}

	unserializer.openObject();

	if(unserializer.key("calibration")&&(unserializer.type()==Type::ARRAY))
	{
		unserializer.openArray();

		size_t index=0;
		while(unserializer.type()==Type::OBJECT)
		{
			CameraParams cameraParams;

			unserializer.openObject();

			cameraParams.model=CalibrationModel::OpenCv_Rational; //assume opencv

			if(unserializer.key("type")&&(unserializer.type()==Type::STRING))
			{
				std::string model=unserializer.getString();

				if(model=="OCam")
					cameraParams.model=CalibrationModel::Ocam;
				else if(model=="Hugin")
					cameraParams.model=CalibrationModel::Hugin;
				else if(model=="CCalib")
					cameraParams.model=CalibrationModel::CCalib;
			}
			else
				messages.push_back(std::string("no camera model type found (assuming OpenCv) for index ")+std::to_string(index));

			std::vector<std::string> lensMessages;

			if(cameraParams.model==CalibrationModel::OpenCv_Rational)
			{
				if(!getOpenCvCameraParameters(unserializer, cameraParams, lensMessages))
					requiredItemMissing=true;
			}
			else if(cameraParams.model==CalibrationModel::Ocam)
			{
				if(!getOcamCameraParameters(unserializer, cameraParams, lensMessages))
					requiredItemMissing=true;
			}
			else if(cameraParams.model==CalibrationModel::Hugin)
			{
				if(!getHuginCameraParameters(unserializer, cameraParams, lensMessages))
					requiredItemMissing=true;
			}
			else if(cameraParams.model==CalibrationModel::CCalib)
			{
				if(!getCCalibCameraParameters(unserializer, cameraParams, lensMessages))
					requiredItemMissing=true;
			}

			if(!lensMessages.empty())
			{
				messages.push_back(std::string("loading lens parameters failed for index ")+std::to_string(index));
				for(std::string &lensMessage:lensMessages)
				{
					messages.push_back(std::string("\t")+lensMessage);
				}
			}

			if(unserializer.key("width"))// && (unserializer.type() == Type::INT))
				cameraParams.width=unserializer.getInt();
			else
				messages.push_back(std::string("no width found for index ")+std::to_string(index));

			if(unserializer.key("height"))// && (unserializer.type() == Type::INT))
				cameraParams.height=unserializer.getInt();
			else
				messages.push_back(std::string("no height found for index ")+std::to_string(index));

			cameraParams.extrinsics.resize(12);

			if(unserializer.key("R")&&(unserializer.type()==Type::OBJECT))
			{
				unserializer.openObject();
				if(unserializer.key("data")&&(unserializer.type()==Type::ARRAY))
				{
					unserializer.openArray();
					for(size_t i=0; i<9; ++i)
						cameraParams.extrinsics[i]=unserializer.getDouble();
					unserializer.closeArray();
				}
				unserializer.closeObject();
			}
			else if(unserializer.key("rot")&&(unserializer.type()==Type::ARRAY))
			{
				unserializer.openArray();
				for(size_t i=0; i<9; ++i)
					cameraParams.extrinsics[i]=unserializer.getDouble();
				unserializer.closeArray();
			}
			else
			{
				requiredItemMissing=true;
				messages.push_back(std::string("no rotation found for index ")+std::to_string(index));
			}

			if(unserializer.key("t")&&(unserializer.type()==Type::OBJECT))
			{
				unserializer.openObject();
				if(unserializer.key("data")&&(unserializer.type()==Type::ARRAY))
				{
					unserializer.openArray();
					for(size_t i=0; i<3; ++i)
						cameraParams.extrinsics[i+9]=unserializer.getDouble();
					unserializer.closeArray();
				}
				unserializer.closeObject();
			}
			else if(unserializer.key("trans")&&(unserializer.type()==Type::ARRAY))
			{
				unserializer.openArray();
				for(size_t i=0; i<3; ++i)
					cameraParams.extrinsics[i+9]=unserializer.getDouble();
				unserializer.closeArray();
			}
			else
			{
				requiredItemMissing=true;
				messages.push_back(std::string("no translation found for index ")+std::to_string(index));
			}

			if(cameraParams.model==CalibrationModel::CCalib) //rotate to world coordinates
			{
				//CCalib uses to fitst cameras as the coordinate system for the remaining cameras with Z axis looking out the camera. We use the same camera model Z out the lens
				//but work in a world coordinates with the xy plane as the horizon and z up. This converts the camera changes the camera resprsentation to account for that.

				glm::mat3 rotation;
				glm::mat3 convertToWorld;

				rotation=calibToRotation(cameraParams.extrinsics);

				convertToWorld[0][0]=0.0f;
				convertToWorld[0][1]=0.0f;
				convertToWorld[0][2]=1.0f;
				convertToWorld[1][0]=-1.0f;
				convertToWorld[1][1]=0.0f;
				convertToWorld[1][2]=0.0f;
				convertToWorld[2][0]=0.0f;
				convertToWorld[2][1]=-1.0f;
				convertToWorld[2][2]=0.0f;

				rotation=rotation*convertToWorld;

				rotationToCalib(rotation, cameraParams.extrinsics);
				
//				glm::mat4 transform;
//				glm::mat4 convertToWorld;
//
//				transform=calibToTransformation(cameraParams.extrinsics);
//
//				convertToWorld[0][0]=0.0f;
//				convertToWorld[0][1]=0.0f;
//				convertToWorld[0][2]=1.0f;
//				convertToWorld[0][3]=0.0f;
//				convertToWorld[1][0]=-1.0f;
//				convertToWorld[1][1]=0.0f;
//				convertToWorld[1][2]=0.0f;
//				convertToWorld[1][3]=0.0f;
//				convertToWorld[2][0]=0.0f;
//				convertToWorld[2][1]=-1.0f;
//				convertToWorld[2][2]=0.0f;
//				convertToWorld[2][3]=0.0f;
//				convertToWorld[3][0]=0.0f;
//				convertToWorld[3][1]=0.0f;
//				convertToWorld[3][2]=0.0f;
//				convertToWorld[3][3]=1.0f;
//
//				transform=transform*convertToWorld;
//				cameraParams.extrinsics=transformationToCalib(transform);

				//				glm::mat3 orgMatrix;
				//				glm::mat3 rotationMatrix;
				//				glm::vec3 t;
				//				glm::mat3 convert;
				//				glm::vec3 converT;
				//		
				//				rotationMatrix[0][0]=cameraParams.extrinsics[0];
				//				rotationMatrix[0][1]=cameraParams.extrinsics[1];
				//				rotationMatrix[0][2]=cameraParams.extrinsics[2];
				//				rotationMatrix[1][0]=cameraParams.extrinsics[3];
				//				rotationMatrix[1][1]=cameraParams.extrinsics[4];
				//				rotationMatrix[1][2]=cameraParams.extrinsics[5];
				//				rotationMatrix[2][0]=cameraParams.extrinsics[6];
				//				rotationMatrix[2][1]=cameraParams.extrinsics[7];
				//				rotationMatrix[2][2]=cameraParams.extrinsics[8];
				//
				//				t[0]=cameraParams.extrinsics[9];
				//				t[1]=cameraParams.extrinsics[10];
				//				t[2]=cameraParams.extrinsics[11];
				//
				//				converT[0]=0;
				//				converT[1]=0;
				//				converT[2]=0;
				//
				//				orgMatrix=rotationMatrix;
				//		
				//				convert[0][0]=0;
				//				convert[0][1]=0;
				//				convert[0][2]=1;
				//				convert[1][0]=-1;
				//				convert[1][1]=0;
				//				convert[1][2]=0;
				//				convert[2][0]=0;
				//				convert[2][1]=-1;
				//				convert[2][2]=0;
				//
				//				rotationMatrix=rotationMatrix*convert;
				//				t=orgMatrix*converT+t;
				//
				//				cameraParams.extrinsics[0]=rotationMatrix[0][0];
				//				cameraParams.extrinsics[1]=rotationMatrix[0][1];
				//				cameraParams.extrinsics[2]=rotationMatrix[0][2];
				//				cameraParams.extrinsics[3]=rotationMatrix[1][0];
				//				cameraParams.extrinsics[4]=rotationMatrix[1][1];
				//				cameraParams.extrinsics[5]=rotationMatrix[1][2];
				//				cameraParams.extrinsics[6]=rotationMatrix[2][0];
				//				cameraParams.extrinsics[7]=rotationMatrix[2][1];
				//				cameraParams.extrinsics[8]=rotationMatrix[2][2];

				//				glm::mat4 rotationMatrix;
				//				glm::mat4 tranformation;
				//
				//				rotationMatrix[0][0]=cameraParams.extrinsics[0];
				//				rotationMatrix[0][1]=cameraParams.extrinsics[1];
				//				rotationMatrix[0][2]=cameraParams.extrinsics[2];
				//				rotationMatrix[0][3]=cameraParams.extrinsics[9];
				//				rotationMatrix[1][0]=cameraParams.extrinsics[3];
				//				rotationMatrix[1][1]=cameraParams.extrinsics[4];
				//				rotationMatrix[1][2]=cameraParams.extrinsics[5];
				//				rotationMatrix[1][3]=cameraParams.extrinsics[10];
				//				rotationMatrix[2][0]=cameraParams.extrinsics[6];
				//				rotationMatrix[2][1]=cameraParams.extrinsics[7];
				//				rotationMatrix[2][2]=cameraParams.extrinsics[8];
				//				rotationMatrix[2][3]=cameraParams.extrinsics[11];
				//				rotationMatrix[3][0]=0.0;
				//				rotationMatrix[3][1]=0.0;
				//				rotationMatrix[3][2]=0.0;
				//				rotationMatrix[3][3]=1.0;
				//
				//				tranformation[0][0]=0.0;
				//				tranformation[0][1]=0.0;
				//				tranformation[0][2]=1.0;
				//				tranformation[0][3]=0.0;
				//				tranformation[1][0]=-1.0;
				//				tranformation[1][1]=0.0;
				//				tranformation[1][2]=0.0;
				//				tranformation[1][3]=0.0;
				//				tranformation[2][0]=0.0;
				//				tranformation[2][1]=-1.0;
				//				tranformation[2][2]=0.0;
				//				tranformation[1][3]=0.0;
				//				tranformation[3][0]=0.0;
				//				tranformation[3][1]=0.0;
				//				tranformation[3][2]=0.0;
				//				tranformation[3][3]=1.0;
				//
				////				rotationMatrix=rotationMatrix*tranformation;
				//				rotationMatrix=tranformation*rotationMatrix;
				//
				//				cameraParams.extrinsics[0]=rotationMatrix[0][0];
				//				cameraParams.extrinsics[1]=rotationMatrix[0][1];
				//				cameraParams.extrinsics[2]=rotationMatrix[0][2];
				//				cameraParams.extrinsics[3]=rotationMatrix[1][0];
				//				cameraParams.extrinsics[4]=rotationMatrix[1][1];
				//				cameraParams.extrinsics[5]=rotationMatrix[1][2];
				//				cameraParams.extrinsics[6]=rotationMatrix[2][0];
				//				cameraParams.extrinsics[7]=rotationMatrix[2][1];
				//				cameraParams.extrinsics[8]=rotationMatrix[2][2];
				//
				//				cameraParams.extrinsics[9]=rotationMatrix[0][3];
				//				cameraParams.extrinsics[10]=rotationMatrix[1][3];
				//				cameraParams.extrinsics[11]=rotationMatrix[2][3];
			}

			m_cameraParams.push_back(cameraParams);

			unserializer.closeObject();

			index++;
		}
		unserializer.closeArray();
	}
	else
	{
		requiredItemMissing=true;
		messages.push_back(std::string("no calibration array found.\n"));
	}

	unserializer.closeObject();

	if(error!=nullptr)
	{
		if(requiredItemMissing)
			(*error)="Failed to load calibration file\n";

		for(std::string &message:messages)
		{
			(*error)+="\t"+message;
		}
	}
	return !requiredItemMissing;
}

bool CalibrationRig::getOpenCvCameraParameters(Unserializer &unserializer, CameraParams &cameraParams, std::vector<std::string> &messages)
{
	bool failed=false;
	cameraParams.intrinsics.resize(12);

	if(unserializer.key("fx")&&(unserializer.type()==Type::DOUBLE))
		cameraParams.intrinsics[0]=unserializer.getDouble();
	else
	{
		failed=true;
		messages.push_back(std::string("fx (x focal point) not found"));
	}
	if(unserializer.key("fy")&&(unserializer.type()==Type::DOUBLE))
		cameraParams.intrinsics[1]=unserializer.getDouble();
	else
	{
		failed=true;
		messages.push_back(std::string("fy (y focal point) not found"));
	}
	if(unserializer.key("ppx")&&(unserializer.type()==Type::DOUBLE))
		cameraParams.intrinsics[2]=unserializer.getDouble();
	else
	{
		failed=true;
		messages.push_back(std::string("ppx (x principal point) not found"));
	}
	if(unserializer.key("ppy")&&(unserializer.type()==Type::DOUBLE))
		cameraParams.intrinsics[3]=unserializer.getDouble();
	else
	{
		failed=true;
		messages.push_back(std::string("ppy (y principal point) found"));
	}
	if(unserializer.key("k1")&&(unserializer.type()==Type::DOUBLE))
		cameraParams.intrinsics[4]=unserializer.getDouble();
	else
	{
		failed=true;
		messages.push_back(std::string("k1 error distortion not found"));
	}
	if(unserializer.key("k2")&&(unserializer.type()==Type::DOUBLE))
		cameraParams.intrinsics[5]=unserializer.getDouble();
	else
	{
		failed=true;
		messages.push_back(std::string("k2 error distortion not found"));
	}
	if(unserializer.key("k3")&&(unserializer.type()==Type::DOUBLE))
		cameraParams.intrinsics[6]=unserializer.getDouble();
	else
	{
		failed=true;
		messages.push_back(std::string("k3 error distortion not found"));
	}
	if(unserializer.key("k4")&&(unserializer.type()==Type::DOUBLE))
		cameraParams.intrinsics[7]=unserializer.getDouble();
	else
	{
		failed=true;
		messages.push_back(std::string("k4 error distortion not found"));
	}
	if(unserializer.key("k5")&&(unserializer.type()==Type::DOUBLE))
		cameraParams.intrinsics[8]=unserializer.getDouble();
	else
	{
		failed=true;
		messages.push_back(std::string("k5 error distortion not found"));
	}
	if(unserializer.key("k6")&&(unserializer.type()==Type::DOUBLE))
		cameraParams.intrinsics[9]=unserializer.getDouble();
	else
	{
		failed=true;
		messages.push_back(std::string("k6 error distortion not found"));
	}
	if(unserializer.key("p1")&&(unserializer.type()==Type::DOUBLE))
		cameraParams.intrinsics[10]=unserializer.getDouble();
	else
	{
		failed=true;
		messages.push_back(std::string("p1 error distortion not found"));
	}
	if(unserializer.key("p2")&&(unserializer.type()==Type::DOUBLE))
		cameraParams.intrinsics[11]=unserializer.getDouble();
	else
	{
		failed=true;
		messages.push_back(std::string("p2 error distortion not found"));
	}

	return !failed;
}

bool CalibrationRig::getOcamCameraParameters(Unserializer &unserializer, CameraParams &cameraParams, std::vector<std::string> &messages)
{
	bool failed=false;

	if(unserializer.key("count")&&(unserializer.type()==Type::DOUBLE))
	{
		int kCount=unserializer.getDouble();

		cameraParams.intrinsics.resize(kCount+6);

		if(unserializer.key("ppx")&&(unserializer.type()==Type::DOUBLE))
			cameraParams.intrinsics[0]=unserializer.getDouble();
		else
		{
			failed=true;
			messages.push_back(std::string("ppx (x principal point) not found"));
		}
		if(unserializer.key("ppy")&&(unserializer.type()==Type::DOUBLE))
			cameraParams.intrinsics[1]=unserializer.getDouble();
		else
		{
			failed=true;
			messages.push_back(std::string("ppy (y principal point) not found"));
		}
		cameraParams.intrinsics[2]=kCount;

		for(int i=0; i<kCount; i++)
		{
			std::string label=(boost::format("k%d")%(i+1)).str();

			if(unserializer.key(label)&&(unserializer.type()==Type::DOUBLE))
				cameraParams.intrinsics[i+3]=unserializer.getDouble();
			else
			{
				failed=true;
				messages.push_back(label+std::string("error distortion not found"));
			}
		}
		if(unserializer.key("c")&&(unserializer.type()==Type::DOUBLE))
			cameraParams.intrinsics[kCount+3]=unserializer.getDouble();
		else
		{
			failed=true;
			messages.push_back(std::string("c error distortion not found"));
		}
		if(unserializer.key("d")&&(unserializer.type()==Type::DOUBLE))
			cameraParams.intrinsics[kCount+4]=unserializer.getDouble();
		else
		{
			failed=true;
			messages.push_back(std::string("d error distortion not found"));
		}
		if(unserializer.key("e")&&(unserializer.type()==Type::DOUBLE))
			cameraParams.intrinsics[kCount+5]=unserializer.getDouble();
		else
		{
			failed=true;
			messages.push_back(std::string("e error distortion not found"));
		}
	}
	else
	{
		failed=true;
		messages.push_back(std::string("no parameter count found"));
	}

	return !failed;
}

bool CalibrationRig::getHuginCameraParameters(Unserializer &unserializer, CameraParams &cameraParams, std::vector<std::string> &messages)
{
	return false;
}

bool CalibrationRig::getCCalibCameraParameters(Unserializer &unserializer, CameraParams &cameraParams, std::vector<std::string> &messages)
{
	bool failed=false;
	cameraParams.intrinsics.resize(9);

	if(unserializer.key("xi")&&(unserializer.type()==Type::DOUBLE))
		cameraParams.intrinsics[0]=unserializer.getDouble();
	else
	{
		failed=true;
		messages.push_back(std::string("xi error distortion not found"));
	}
	if(unserializer.key("fx")&&(unserializer.type()==Type::DOUBLE))
		cameraParams.intrinsics[1]=unserializer.getDouble();
	else
	{
		failed=true;
		messages.push_back(std::string("fx (x focal point) not found"));
	}
	if(unserializer.key("fy")&&(unserializer.type()==Type::DOUBLE))
		cameraParams.intrinsics[2]=unserializer.getDouble();
	else
	{
		failed=true;
		messages.push_back(std::string("fx (x focal point) not found"));
	}
	if(unserializer.key("k1")&&(unserializer.type()==Type::DOUBLE))
		cameraParams.intrinsics[3]=unserializer.getDouble();
	else
	{
		failed=true;
		messages.push_back(std::string("k1 error distortion not found"));
	}
	if(unserializer.key("k2")&&(unserializer.type()==Type::DOUBLE))
		cameraParams.intrinsics[4]=unserializer.getDouble();
	else
	{
		failed=true;
		messages.push_back(std::string("k2 error distortion not found"));
	}
	if(unserializer.key("k3")&&(unserializer.type()==Type::DOUBLE))
		cameraParams.intrinsics[5]=unserializer.getDouble();
	else
	{
		failed=true;
		messages.push_back(std::string("k3 error distortion not found"));
	}
	if(unserializer.key("k4")&&(unserializer.type()==Type::DOUBLE))
		cameraParams.intrinsics[6]=unserializer.getDouble();
	else
	{
		failed=true;
		messages.push_back(std::string("k4 error distortion not found"));
	}
	if(unserializer.key("ppx")&&(unserializer.type()==Type::DOUBLE))
		cameraParams.intrinsics[7]=unserializer.getDouble();
	else
	{
		failed=true;
		messages.push_back(std::string("ppx (x principal point) not found"));
	}
	if(unserializer.key("ppy")&&(unserializer.type()==Type::DOUBLE))
		cameraParams.intrinsics[8]=unserializer.getDouble();
	else
	{
		failed=true;
		messages.push_back(std::string("ppy (y principal point) not found"));
	}

	return !failed;
}

void CalibrationRig::openHuginCalib(std::string location)
{
	std::ifstream file(location);

	if(!file)
		return;

	std::string line;

	while(std::getline(file, line))
	{
		if(line.empty())
			continue;

		if(line[0]=='i')
		{
			std::vector<std::string> params;
			std::istringstream lineStream(line);
			std::string param;

			while(std::getline(lineStream, param, ' '))
			{
				params.push_back(param);
			}

			if(!params.empty())
			{
				CameraParams cameraParams;

				cameraParams.model=CalibrationModel::Hugin;
				cameraParams.intrinsics.resize(7);
				cameraParams.extrinsics.resize(12);

				float roll, pitch, yaw;

				for(size_t i=0; i<params.size(); ++i)
				{
					std::string &param=params[i];

					switch(param[0])
					{
					case 'w':
						cameraParams.width=std::stoi(param.substr(1));
						break;
					case 'h':
						cameraParams.height=std::stoi(param.substr(1));
						break;
					case 'f':
						cameraParams.intrinsics[0]=std::stoi(param.substr(1));
						break;
					case 'a':
						cameraParams.intrinsics[1]=std::stof(param.substr(1));
						break;
					case 'b':
						cameraParams.intrinsics[2]=std::stof(param.substr(1));
						break;
					case 'c':
						cameraParams.intrinsics[3]=std::stof(param.substr(1));
						break;
					case 'd':
						cameraParams.intrinsics[5]=std::stof(param.substr(1));
						break;
					case 'e':
						cameraParams.intrinsics[6]=std::stof(param.substr(1));
						break;
					case 'r':
						roll=std::stof(param.substr(1));
						break;
					case 'p':
						pitch=std::stof(param.substr(1));
						break;
					case 'y':
						yaw=std::stof(param.substr(1));
						break;
					case 'T':
					{
						switch(param[1])
						{
						case 'r':
						{
							switch(param[2])
							{
							case 'X':
								cameraParams.extrinsics[9]=std::stof(param.substr(3));
								break;
							case 'Y':
								cameraParams.extrinsics[10]=std::stof(param.substr(3));
								break;
							case 'Z':
								cameraParams.extrinsics[11]=std::stof(param.substr(3));
								break;
							}
						}
						break;
						}
					}
					break;
					}
				}

				cameraParams.intrinsics[4]=1.0f-cameraParams.intrinsics[1]+cameraParams.intrinsics[2]+cameraParams.intrinsics[3];

				glm::mat4 rotationMatrix=glm::eulerAngleYXZ(yaw, pitch, roll);

				cameraParams.extrinsics[0]=rotationMatrix[0][0];
				cameraParams.extrinsics[1]=rotationMatrix[0][1];
				cameraParams.extrinsics[2]=rotationMatrix[0][2];
				cameraParams.extrinsics[3]=rotationMatrix[1][0];
				cameraParams.extrinsics[4]=rotationMatrix[1][1];
				cameraParams.extrinsics[5]=rotationMatrix[1][2];
				cameraParams.extrinsics[6]=rotationMatrix[2][0];
				cameraParams.extrinsics[7]=rotationMatrix[2][1];
				cameraParams.extrinsics[8]=rotationMatrix[2][2];

				m_cameraParams.push_back(cameraParams);

			}
		}
	}

	file.close();
}

void CalibrationRig::save(std::string location)
{}

}//namespace Limitless