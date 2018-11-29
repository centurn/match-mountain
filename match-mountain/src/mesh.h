#pragma once

#include "asg_base.h"
#include <memory>
#include "attrib_desc.h"
#include "draw_desc.h"

namespace asg{

class ShaderProgram;

class Mesh{
public:
    Mesh();

    template<typename T>// attr is universal reference to AttribDescription
    size_t addAttribute(T&& attr);

    template<typename T>
    void setDrawDescription(DrawDescr desc);

    inline void setProgram(const std::shared_ptr<ShaderProgram>& prog);
    inline const std::shared_ptr<ShaderProgram>& getProgram() const;

    void draw();

private:
    void prepareGL();

private:
    std::shared_ptr<ShaderProgram> program;
    std::vector<AttribDescr> attribs;
    DrawDescr draw_desc;

    uint vao = 0;
};

template<typename T>
size_t Mesh::addAttribute(T&& attr){
    attribs.emplace_back(attr);
    return attribs.size() - 1;
}

template<typename T>
void Mesh::setDrawDescription(DrawDescr desc){
    draw_desc = desc;
}

void Mesh::setProgram(const std::shared_ptr<ShaderProgram>& prog){
    program = prog;
}

const std::shared_ptr<ShaderProgram>& Mesh::getProgram() const
{
    return program;
}

}
