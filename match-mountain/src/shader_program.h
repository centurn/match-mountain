#pragma once

#include "mam_base.h"

struct ShaderID: public ResourceGL<ShaderID>{};
struct ProgramID: public ResourceGL<ProgramID>{};

class ShaderProgram{
public:
    ShaderProgram();
    ~ShaderProgram();

    // Separate init func to ensure it runs in GL context
    void init(const char* vs, const char* fs);
    void bind();
    uint getID() const{
        return program.id;
    }

private:
    static void compile(ShaderID& dest, const char* source, uint shaderType);

    ShaderID vs;
    ShaderID fs;
    ProgramID program;
};
