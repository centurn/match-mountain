#pragma once

#include "asg_base.h"
#include "asg_render_predef.h"

namespace asg{
struct ShaderID: public ResourceGL<ShaderID>{};
struct ProgramID: public ResourceGL<ProgramID>{};

class ShaderProgram{
public:
    ShaderProgram(const char* vs, const char* fs);
    ~ShaderProgram();

    // Separate init func to ensure it runs in GL context
    void bind();
    Ruint getID() const{
        return program.id;
    }

private:
    void prepareGL();
    static void compile(ShaderID& dest, const char* source, uint shaderType);

    std::string vs_src;
    std::string fs_src;

    ShaderID vs;
    ShaderID fs;
    ProgramID program;
};
}
