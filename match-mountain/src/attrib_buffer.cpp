#include "attrib_buffer.h"
#include "asg_gl.h"

namespace asg{

DEFINE_GL_ARRAY_DELETER(BufferID, glDeleteBuffers);

AttribBuffer::AttribBuffer(const byte *src, size_t extent)
{
    buff.resize(extent);
    std::memcpy(buff.data(), src, extent);
}

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
    if(id_gl.id[0] == 0)
        init();
    glBindBuffer(GL_ARRAY_BUFFER, id_gl.id[0]);checkGL();    
}


}
