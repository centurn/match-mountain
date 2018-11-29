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

    // attr is universal reference to AttribDescription
    template<typename T>
    size_t addAttribute(T&& attr){
        attribs.emplace_back(attr);
        return attribs.size() - 1;
    }

    template<typename T>
    void setDrawDescription(DrawDescr desc){
        draw_desc = desc;
    }

    void setProgram(const std::shared_ptr<ShaderProgram>& prog){
        program = prog;
    }

    void draw();

private:
    void prepareGL();

public:
    std::shared_ptr<ShaderProgram> program;
    std::vector<AttribDescr> attribs;
    DrawDescr draw_desc;

    uint vao = 0;
};

}
