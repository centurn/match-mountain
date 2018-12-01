#include "shader_program.h"

#include "asg_gl.h"

namespace asg{
DEFINE_GL_DELETER(ShaderID, glDeleteShader);
DEFINE_GL_DELETER(ProgramID, glDeleteProgram);

ShaderProgram::ShaderProgram(const char* vs, const char* fs)
    : vs_src(vs)
    , fs_src(fs)
{
}

ShaderProgram::~ShaderProgram()
{

}

void ShaderProgram::bind()
{
    prepareGL();
    glUseProgram(program.id);checkGL();
}

void ShaderProgram::prepareGL()
{
    if(program.id != 0)
        return;
    compile(vs, vs_src.c_str(), GL_VERTEX_SHADER);
    compile(fs, fs_src.c_str(), GL_FRAGMENT_SHADER);

    // Link the vertex and fragment shader into a shader program
    program.id = glCreateProgram();checkGL();
    glAttachShader(program.id, vs.id);checkGL();
    glAttachShader(program.id, fs.id);checkGL();
    glLinkProgram(program.id);checkGL();

#ifdef ENABLE_GL_CHECKS
    GLint linkResult;
    glGetProgramiv(program.id, GL_LINK_STATUS, &linkResult);

    VALIDATE(linkResult != GL_FALSE);
    glValidateProgram(program.id);
    glGetProgramiv(program.id, GL_VALIDATE_STATUS, &linkResult);
    if (linkResult == GL_FALSE) {
        GLsizei log_length = 0;
        GLchar message[1024];
        glGetProgramInfoLog(program.id, sizeof(message), &log_length, message);
        log_e("Error validating shader: %s\n", message);
        FAIL();
    }
#endif
}

void ShaderProgram::compile(ShaderID& dest, const char *source, uint shaderType)
{
    dest.id = glCreateShader(shaderType);
    glShaderSource(dest.id, 1, &source, nullptr);checkGL();
    glCompileShader(dest.id);

#ifdef ENABLE_GL_CHECKS
    GLint compile_result;
    glGetShaderiv(dest.id, GL_COMPILE_STATUS, &compile_result);
    if (compile_result != GL_TRUE) {
        GLsizei log_length = 0;
        GLchar message[1024];
        glGetShaderInfoLog(dest.id, sizeof(message), &log_length, message);
        log_e("VS compile failed error: %s\n", message);
        FAIL();
    }
#endif
}
}
