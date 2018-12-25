#pragma once

#include "attrib_desc.h"
#include "draw_desc.h"
#include "uniforms.h"
#include <memory>

namespace asg{

class ShaderProgram;

class Mesh{
public:
    Mesh();
    ~Mesh();

    template<typename T>// attr is universal reference to AttribDescription
    size_t addAttribute(T&& attr);

    template<typename T>
    void setDrawDescription(T&& desc);

    inline void setProgram(const std::shared_ptr<ShaderProgram>& prog);
    inline const std::shared_ptr<ShaderProgram>& getProgram() const;

    void setTexture(const std::string& name, const std::shared_ptr<Texture>& tex);

    inline UniformHandler addUniform(const std::string name);

    inline void makeDirty();

    void render();

private:
    void prepareGL();

private:
    std::shared_ptr<ShaderProgram> program;
    std::vector<AttribDescr> attribs;
    UniformStorage uniforms;
    DrawDescr draw_desc;

    uint vao = 0;
    bool dirty = true;
};

using MeshVector = std::vector<std::unique_ptr<asg::Mesh>>;

template<typename T>
size_t Mesh::addAttribute(T&& attr){
    attribs.emplace_back(attr);
    return attribs.size() - 1;
}

template<typename T>
void Mesh::setDrawDescription(T&& desc){
    draw_desc = desc;
}

void Mesh::setProgram(const std::shared_ptr<ShaderProgram>& prog){
    program = prog;
}

const std::shared_ptr<ShaderProgram>& Mesh::getProgram() const
{
    return program;
}

UniformHandler Mesh::addUniform(const std::string name)
{
    return uniforms.add(name);
}

void Mesh::makeDirty()
{
    dirty = true;
}

}
