#include "attrib_buffer.h"
#include "asg_gl.h"

namespace asg{

DEFINE_GL_ARRAY_DELETER(BufferID, glDeleteBuffers);

AttribBuffer::AttribBuffer(const byte *src, size_t extent, bool is_vertex)
    : is_vertex(is_vertex)
{
    buff.resize(extent);
    std::memcpy(buff.data(), src, extent);
}

AttribBuffer::AttribBuffer(std::vector<byte> &&src, bool is_vertex)
    : is_vertex(is_vertex)
{
    buff = src;
}

AttribBuffer::~AttribBuffer()
{
}

void AttribBuffer::init()
{
    glGenBuffers(1, id_gl.id);checkGL();
    glBindBuffer(is_vertex? GL_ARRAY_BUFFER : GL_ELEMENT_ARRAY_BUFFER
                 , id_gl.id[0]);checkGL();
    glBufferData(is_vertex? GL_ARRAY_BUFFER : GL_ELEMENT_ARRAY_BUFFER
                 , static_cast<GLsizeiptr>(buff.size())
                 , buff.data()
                 , GL_STATIC_DRAW);checkGL();
}

void AttribBuffer::bind()
{
    if(id_gl.id[0] == 0)
        init();
    glBindBuffer(is_vertex? GL_ARRAY_BUFFER : GL_ELEMENT_ARRAY_BUFFER
                 , id_gl.id[0]);checkGL();
}


}
