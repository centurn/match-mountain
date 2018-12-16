
#include "uniforms.h"
#include "asg_gl.h"
#include "glm/gtc/type_ptr.hpp"
#include <algorithm>

namespace asg{

namespace{
struct ApplyToGL{
    Rint gl_id;
    Ruint& num_tex_units_used;

    void operator()(float a){
        glUniform1f(gl_id, a);
    }
    void operator()(const vec2& a){
        glUniform2fv(gl_id, 1, glm::value_ptr(a));
    }
    void operator()(const vec3& a){
        glUniform3fv(gl_id, 1, glm::value_ptr(a));
    }
    void operator()(const vec4& a){
        glUniform4fv(gl_id, 1, glm::value_ptr(a));
    }
    void operator()(const glm::mat4& a){
        glUniformMatrix4fv(gl_id, 1, GL_FALSE, glm::value_ptr(a));
    }
    void operator()(const std::shared_ptr<Texture>& tex){
        glActiveTexture(GL_TEXTURE0 + num_tex_units_used);checkGL();
        tex->bind();
        glUniform1i(gl_id, int(num_tex_units_used));checkGL();
        ++num_tex_units_used;
    }
};
}


void UniformStorage::apply(Ruint program)
{
    prepareGL(program);
    Ruint num_textures = 0;
    for(auto& i: data){
        std::visit(ApplyToGL{i.gl_id, num_textures}, i.data);
    }
}

UniformHandler UniformStorage::add(const std::string &name)
{
    auto existing = std::find_if(data.begin(), data.end(), [&name](auto a){
        return a.name == name;
    });
    if(existing != data.end())
        return {static_cast<size_t>(existing - data.begin()), this};

    data.emplace_back(name);
    return {data.size() - 1, this};
    update_gl = true;
}

void UniformStorage::prepareGL(Ruint program)
{
    if(!update_gl)
        return;
    update_gl = false;

    for(auto& i: data){
        i.gl_id = glGetUniformLocation(program, i.name.c_str());checkGL();
        VALIDATE(i.gl_id != -1);
    }
}

}
