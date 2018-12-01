#include "mesh.h"

#include "asg_gl.h"
#include "shader_program.h"

namespace asg{

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
}

void Mesh::setTexture(const std::string &name, const std::shared_ptr<Texture> &tex)
{
    auto uni = uniforms.add(name);
    uni.set(tex);
}

void Mesh::draw()
{
    prepareGL();

    glBindVertexArray(vao);
    program->bind();
    uniforms.apply(program->getID());
}

void Mesh::prepareGL()
{
    if(vao != 0)
        return;// Assume it's properly initialized already

    glGenVertexArrays(1, &vao);checkGL();
    glBindVertexArray(vao);checkGL();

    program->bind();
    for(const auto& i: attribs){
        GLuint attrib_id = static_cast<GLuint>(
                           glGetAttribLocation(program->getID(), i.name.c_str())); checkGL();

        i.buff->bind();
        glEnableVertexAttribArray(attrib_id);
        glVertexAttribPointer(attrib_id
                              , i.num_components
                              , static_cast<GLenum>(i.type)
                              , GL_FALSE
                              , static_cast<GLsizei>(i.stride)
                              , reinterpret_cast<const void *>(i.offset)); checkGL();
    }
}

}
