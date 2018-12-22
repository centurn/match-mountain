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

    template<class T, std::ptrdiff_t Extent>
    void set(span<T, Extent> data);

    void bind();
    inline const byte* data() const;
private:
    AttribBuffer(const byte* src, size_t extent, bool is_vertex);

    void set(const byte* src, size_t extent);
    void init();
    std::vector<byte> buff;
    BufferID id_gl;
    bool is_vertex;
    bool dirty = true;// Needs to upload data to GL
};

const byte* AttribBuffer::data() const
{
    return buff.data();
}

template<class T, std::ptrdiff_t Extent>
AttribBuffer::AttribBuffer(span<T, Extent> src, bool is_vertex)
    : AttribBuffer{reinterpret_cast<const byte*>(src.data()), src.size_bytes(), is_vertex}
{
}

template<class T, std::ptrdiff_t Extent>
void AttribBuffer::set(span<T, Extent> data){
    set(reinterpret_cast<const byte*>(data.data()), data.size_bytes());
}

}// namespace asg
