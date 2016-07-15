#ifndef _CalibrationRig_h_
#define _CalibrationRig_h_

#include <string>
#include <vector>

#include "Base/Attributes.h"

#include "Utilities/utilities_define.h"

namespace Limitless
{

enum class UTILITIES_EXPORT CalibrationModel
{
	OpenCv_Rational=0,
	Ocam=1,
	CCalib=2,
	Hugin=3
};

struct UTILITIES_EXPORT CameraParams
{
	CalibrationModel model;

	unsigned int width;
	unsigned int height;
	std::vector<float> extrinsics;
	std::vector<float> intrinsics;
};

class UTILITIES_EXPORT CalibrationRig
{
public:
	CalibrationRig();
	~CalibrationRig();

	virtual std::string name() { return m_name; }
	virtual std::string location() { return m_location; }

	virtual bool load(std::string location, std::string *error=nullptr);
	virtual void save(std::string location);

	std::vector<CameraParams> &cameraParams() { return m_cameraParams; }

	bool isLoaded() { return m_loaded; }

private:
	bool loadCameraParams(Unserializer &unserializer, std::string *error);
	bool getOpenCvCameraParameters(Unserializer &unserializer, CameraParams &cameraParams, std::vector<std::string> &messages);
	bool getOcamCameraParameters(Unserializer &unserializer, CameraParams &cameraParams, std::vector<std::string> &messages);
	bool getHuginCameraParameters(Unserializer &unserializer, CameraParams &cameraParams, std::vector<std::string> &messages);
	bool getCCalibCameraParameters(Unserializer &unserializer, CameraParams &cameraParams, std::vector<std::string> &messages);

	void openHuginCalib(std::string location);

	std::string m_name;
	std::string m_location;

	bool m_loaded;

	double m_reprojectionError;
	std::vector<CameraParams> m_cameraParams;
};

}//namespace Limitless

#endif //_CalibrationRig_h_