#pragma once

#if defined(_WIN32) || defined(__CYGWIN__)
#    define OVR_OS_WIN32
#    include <windows.h>
#elif defined(__APPLE__)
#    define OVR_OS_MAC
#else
#    define OVR_OS_LINUX
#endif

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


static const float PI = 3.14159265358979323846f;

inline unsigned int next_pow2(unsigned int x)
{
    x -= 1;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return x + 1;
}

/**
 * Float approximate-equality comparison function
 */
template<typename T>
inline bool fequal(T a, T b, T epsilon = 0.0001){
    if (a == b) {
        // Shortcut
        return true;
    }

    const T diff = std::abs(a - b);
    if (a * b == 0) {
        // a or b or both are zero; relative error is not meaningful here
        return diff < (epsilon * epsilon);
    }

    return diff / (std::abs(a) + std::abs(b)) < epsilon;
}

namespace la {
    using namespace glm;

    template<typename genType>
    inline std::ostream& operator<<(std::ostream& out, const genType& g)
    {
        return out << glm::to_string(g);
    }

    inline QMatrix4x4 to_qmat(const mat4 &m)
    {
        return QMatrix4x4(value_ptr(transpose(m)));
    }

    inline glm::vec3 from_vec3f(const Vec3f &v)
    {
        return glm::vec3(v[0], v[1], v[2]);
    }
}

void printGLErrorLog();
void printLinkInfoLog(int prog);
void printShaderInfoLog(int shader);
void debugContextVersion();
