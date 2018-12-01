
#include "uniforms.h"
#include "asg_gl.h"

namespace asg{

namespace{
struct ApplyToGL{
    Rint gl_id;
    Ruint& num_tex_units_used;

    void operator()(const std::shared_ptr<Texture> tex){
        glActiveTexture(GL_TEXTURE0 + num_tex_units_used);checkGL();
        tex->bind();
        glUniform1i(gl_id, int(num_tex_units_used));checkGL();
        ++num_tex_units_used;
    }
    template<typename T>
    void operator()(T&& ){
        std::abort();// not implemented
    }
};
}


void UniformStorage::apply(Ruint program)
{
    Ruint num_textures = 0;
    for(auto& i: data){
        std::visit(ApplyToGL{i.gl_id, num_textures}, i.data);
    }
    prepareGL(program);
}

UniformHandler UniformStorage::add(const std::string &name)
{
    data.emplace_back(name);
    return {data.size() - 1, this};
}

void UniformStorage::prepareGL(Ruint program)
{
    if(!update_gl)
        return;

    for(auto& i: data){
        i.gl_id = glGetUniformLocation(program, i.name.c_str());checkGL();
        FAIL_IF(i.gl_id == -1);
    }
}

}
