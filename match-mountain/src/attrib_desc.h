#pragma once

#include "attrib_buffer.h"
#include <memory>
#include "asg_render_predef.h"
#include "glm/vec3.hpp"
#include "glm/vec2.hpp"

namespace asg {

template <int N>
struct DeduceScalarType<glm::vec<N, float, glm::defaultp>>{
    static constexpr ScalarType scalar_type = ScalarType::Float;
};

struct AttribDescr{
    std::string name;
    std::shared_ptr<AttribBuffer> buff;
    Rint num_components;
    ScalarType type;
    ptrdiff_t offset;
    ptrdiff_t stride;

    template<typename T, int N>
    static AttribDescr fromArray(const std::string name
                                 , T (&src)[N]
                                 , Rint num_components){
        return AttribDescr{
              name
            , std::make_shared<AttribBuffer>(make_span(src))
            , num_components
            , DeduceScalarType<typename std::remove_cv<T>::type>::scalar_type
            , 0
            , 0
        };
    }
};

}
