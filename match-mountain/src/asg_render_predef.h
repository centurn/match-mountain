# pragma once

// This file contains some pre-defined basic types that match OpenGL predefines
// This is to avoid including GL headers where it can be avoided

#include "asg_base.h"

namespace asg{

using Rint = int;//GLint
using Ruint = unsigned int;// GLuint
using Renum = unsigned int;// GLenum
using Rushort = unsigned short;// GLushort
using Rshort = short;// GLshort
using Rsize = int;//GLSizei

enum class ScalarType : Renum{
  Byte = 0x1400 // GL_BYTE
, UByte = 0x1401 // GL_UNSIGNED_BYTE
, Short = 0x1402 // GL_SHORT
, UShort = 0x1403 // GL_UNSIGNED_SHORT
, Int = 0x1404 // GL_INT
, UInt = 0x1405 // GL_UNSIGNED_INT
, Float = 0x1406 // GL_FLOAT
};


// Type traits to determine the OpenGL type by passed array contents
template <typename T>
struct DeduceScalarType{
};

template <>
struct DeduceScalarType<float>{
    static constexpr Rint dimension = 1;
    static constexpr ScalarType scalar_type = ScalarType::Float;
};

template <>
struct DeduceScalarType<Rint>{
    static constexpr Rint dimension = 1;
    static constexpr ScalarType scalar_type = ScalarType::Int;
};

template <>
struct DeduceScalarType<Rushort>{
    static constexpr Rint dimension = 1;
    static constexpr ScalarType scalar_type = ScalarType::UShort;
};

}
