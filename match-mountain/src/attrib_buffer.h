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
    IMPLICIT inline AttribBuffer(span<T, Extent> data);

    inline IMPLICIT AttribBuffer(std::vector<byte>&& src);
    ~AttribBuffer();

    // Initalizes GL state. Should be called while GL context is bound
    void init();
    void bind();
private:
    AttribBuffer(const byte* src, size_t extent);
    std::vector<byte> buff;
    BufferID id_gl;
};

template<class T, std::ptrdiff_t Extent>
AttribBuffer::AttribBuffer(span<T, Extent> src)
    : AttribBuffer{reinterpret_cast<const byte*>(src.data()), src.size_bytes()}
{
}

}// namespace asg
