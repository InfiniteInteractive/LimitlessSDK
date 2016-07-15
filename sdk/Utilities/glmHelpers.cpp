#include "glmHelpers.h"

namespace Limitless
{

glm::mat3 calibToRotation(std::vector<float> &params)
{
	glm::mat3 mat;

	mat[0][0]=params[0];
	mat[1][0]=params[1];
	mat[2][0]=params[2];
	mat[0][1]=params[3];
	mat[1][1]=params[4];
	mat[2][1]=params[5];
	mat[0][2]=params[6];
	mat[1][2]=params[7];
	mat[2][2]=params[8];

	return mat;
}

void rotationToCalib(glm::mat3 &mat, std::vector<float> &params)
{
	params[0]=mat[0][0];
	params[1]=mat[1][0];
	params[2]=mat[2][0];
	params[3]=mat[0][1];
	params[4]=mat[1][1];
	params[5]=mat[2][1];
	params[6]=mat[0][2];
	params[7]=mat[1][2];
	params[8]=mat[2][2];
}

glm::vec3 calibToTranslation(std::vector<float> &params)
{
	glm::vec3 vec;

	vec[0]=params[9];
	vec[1]=params[10];
	vec[2]=params[11];

	return vec;
}

void translationToCalib(glm::vec3 &vec, std::vector<float> &params)
{
	params[9]=vec[0];
	params[10]=vec[1];
	params[11]=vec[2];
}

glm::mat4 calibToTransformation(std::vector<float> &params)
{
	glm::mat4 mat;

	mat[0][0]=params[0];
	mat[1][0]=params[1];
	mat[2][0]=params[2];
	mat[3][0]=params[9];
	mat[0][1]=params[3];
	mat[1][1]=params[4];
	mat[2][1]=params[5];
	mat[3][1]=params[10];
	mat[0][2]=params[6];
	mat[1][2]=params[7];
	mat[2][2]=params[8];
	mat[3][2]=params[11];
	mat[0][3]=0.0f;
	mat[1][3]=0.0f;
	mat[2][3]=0.0f;
	mat[3][3]=1.0f;

	return mat;
}

void transformationToCalib(glm::mat4 &transform, std::vector<float> &params)
{
	params[0]=transform[0][0];
	params[1]=transform[1][0];
	params[2]=transform[2][0];
	params[9]=transform[3][0];
	params[3]=transform[0][1];
	params[4]=transform[1][1];
	params[5]=transform[2][1];
	params[10]=transform[3][1];
	params[6]=transform[0][2];
	params[7]=transform[1][2];
	params[8]=transform[2][2];
	params[11]=transform[3][2];
}

}//namespace Limitless

