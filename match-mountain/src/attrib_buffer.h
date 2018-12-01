#pragma once

#include "asg_base.h"

#include <vector>
#include <cstring>
//

namespace asg{

struct BufferID: public ResourceArrayGL<1, BufferID>{};

class AttribBuffer{
public:
    template<class T, std::ptrdiff_t Extent>
    IMPLICIT inline AttribBuffer(span<T, Extent> data, bool is_vertex = true);

    IMPLICIT AttribBuffer(std::vector<byte>&& src, bool is_vertex = true);
    ~AttribBuffer();

    // Initalizes GL state. Should be called while GL context is bound
    void init();
    void bind();
private:
    AttribBuffer(const byte* src, size_t extent, bool is_vertex);
    std::vector<byte> buff;
    bool is_vertex;
    BufferID id_gl;
};

template<class T, std::ptrdiff_t Extent>
AttribBuffer::AttribBuffer(span<T, Extent> src, bool is_vertex)
    : AttribBuffer{reinterpret_cast<const byte*>(src.data()), src.size_bytes(), is_vertex}
{
}

}// namespace asg
