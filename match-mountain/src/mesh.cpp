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

void Mesh::render()
{
    prepareGL();

    glBindVertexArray(vao);
    program->bind();
    uniforms.apply(program->getID());
    // TODO: Index Buffer
    glDrawArrays(GLuint(draw_desc.draw_type)
                 , draw_desc.offset
                 , draw_desc.count);checkGL();
}

void Mesh::prepareGL()
{
    if(vao != 0)
        return;// Assume it's properly initialized already

    glGenVertexArrays(1, &vao);checkGL();
    glBindVertexArray(vao);checkGL();

    program->bind();
    for(const auto& i: attribs){
        GLint attrib_id = glGetAttribLocation(program->getID(), i.name.c_str()); checkGL();
        VALIDATE(attrib_id != -1);

        i.buff->bind();
        glEnableVertexAttribArray(static_cast<GLuint>(attrib_id));
        glVertexAttribPointer(static_cast<GLuint>(attrib_id)
                              , i.num_components
                              , static_cast<GLenum>(i.type)
                              , GL_FALSE
                              , static_cast<GLsizei>(i.stride)
                              , reinterpret_cast<const void *>(i.offset)); checkGL();
    }
}

}
