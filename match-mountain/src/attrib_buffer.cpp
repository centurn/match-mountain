#include "attrib_buffer.h"
#include "asg_gl.h"

namespace asg{

DEFINE_GL_ARRAY_DELETER(BufferID, glDeleteBuffers);

AttribBuffer::AttribBuffer(std::vector<byte> &&src)
{
    buff = src;
}

AttribBuffer::~AttribBuffer()
{
}

void AttribBuffer::init()
{
    glGenBuffers(1, id_gl.id);checkGL();
    glBindBuffer(GL_ARRAY_BUFFER, id_gl.id[0]);checkGL();
    glBufferData(GL_ARRAY_BUFFER
                 , static_cast<GLsizeiptr>(buff.size())
                 , buff.data()
                 , GL_STATIC_DRAW);checkGL();
}

void AttribBuffer::bind()
{
    //glBindBuffer(GL_ARRAY_BUFFER, id_gl.id[0]);checkGL();
}

AttribBuffer::AttribBuffer(const byte *src, size_t extent)
{
    buff.resize(extent);
    std::memcpy(buff.data(), src, extent);
}

}
