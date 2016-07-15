#ifndef _utilities_glmHelper_h_
#define _utilities_glmHelper_h_

#include "utilities_define.h"

#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <vector>

namespace Limitless
{

UTILITIES_EXPORT glm::mat3 calibToRotation(std::vector<float> &params);
UTILITIES_EXPORT void rotationToCalib(glm::mat3 &mat, std::vector<float> &params);

UTILITIES_EXPORT glm::vec3 calibToTranslation(std::vector<float> &params);
UTILITIES_EXPORT void translationToCalib(glm::vec3 &vec, std::vector<float> &params);

UTILITIES_EXPORT glm::mat4 calibToTransformation(std::vector<float> &params);
UTILITIES_EXPORT void transformationToCalib(glm::mat4 &transform, std::vector<float> &params);

}//namespace Limitless

#endif //_utilities_glmHelper_h_