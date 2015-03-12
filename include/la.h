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

#include <3dGrid/vec.h>
#include <algorithm>
#include <string>
#include <cmath>


static const float PI = 3.14159265358979323846f;

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

    inline glm::vec3 from_vec3d(const Vec3d &v)
    {
        return glm::vec3(v[0], v[1], v[2]);
    }
}
