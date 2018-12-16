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
    static constexpr Rint dimension = N;
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
                                 , T (&src)[N]){
        return AttribDescr{
              name
            , std::make_shared<AttribBuffer>(make_span(src))
            , DeduceScalarType<typename std::remove_cv<T>::type>::dimension
            , DeduceScalarType<typename std::remove_cv<T>::type>::scalar_type
            , 0
            , 0
        };
    }

    template<typename T, typename C>
    static AttribDescr fromStruct(const std::string&name
                             , const std::shared_ptr<AttribBuffer>&buff
                             , ptrdiff_t offset
                             , T C::*){
        return AttribDescr{
              name
            , buff
            , DeduceScalarType<typename std::remove_cv<T>::type>::dimension
            , DeduceScalarType<typename std::remove_cv<T>::type>::scalar_type
            , offset
            , sizeof(C)
        };
    }
};

}

// Avoid-copypaste macro to add attributes that form 'array-of structs'
#define ASG_STRUCT_ATTRIBUTE(MESH, BUFFER, STRUCT, ATTRIBUTE) \
    (MESH).addAttribute(AttribDescr::fromStruct(\
                            #ATTRIBUTE\
                          , (BUFFER)\
                          , offsetof(STRUCT, ATTRIBUTE)\
                          , &STRUCT::ATTRIBUTE));\

