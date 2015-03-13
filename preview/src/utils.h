#pragma once

#define NOMINMAX 1

#define GLM_FORCE_RADIANS

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <QMatrix4x4>
#include <3dGrid/vec.h>
#include <algorithm>
#include <string>
#include <cmath>

#include <la.h>

namespace la
{
inline QMatrix4x4 to_qmat(const mat4 &m)
{
  return QMatrix4x4(value_ptr(transpose(m)));
}
}

void printGLErrorLog();
void printLinkInfoLog(int prog);
void printShaderInfoLog(int shader);
void debugContextVersion();
