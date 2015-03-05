#ifndef LA
#define LA

#include <QMatrix4x4>

// NOTE: This #define forces GLM to use radians (not degrees) for ALL of its
// angle arguments. The documentation may not always reflect this fact.
#define GLM_FORCE_RADIANS
#define GLM_277  // Don't copy this include!
#include <glm/glm.hpp>
#undef GLM_277

// For glm::translate, glm::rotate, and glm::scale.
#include <glm/gtc/matrix_transform.hpp>
// For glm::to_string.
#include <glm/gtx/string_cast.hpp>
// For glm::value_ptr.
#include <glm/gtc/type_ptr.hpp>


/// 277 linear algebra namespace
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
}


#endif // LA
