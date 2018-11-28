#include "shader_program.h"

#include "asg_gl.h"

namespace asg{
DEFINE_GL_DELETER(ShaderID, glDeleteShader);
DEFINE_GL_DELETER(ProgramID, glDeleteProgram);

ShaderProgram::ShaderProgram()
{
}

ShaderProgram::~ShaderProgram()
{

}

void ShaderProgram::init(const char *vert_source, const char * frag_source)
{
    compile(vs, vert_source, GL_VERTEX_SHADER);
    compile(fs, frag_source, GL_FRAGMENT_SHADER);

    // Link the vertex and fragment shader into a shader program
    program.id = glCreateProgram();checkGL();
    glAttachShader(program.id, vs.id);checkGL();
    glAttachShader(program.id, fs.id);checkGL();
    glLinkProgram(program.id);checkGL();

#ifdef ENABLE_GL_CHECKS
    GLint linkResult;
    glGetProgramiv(program.id, GL_LINK_STATUS, &linkResult);

    if (linkResult == GL_FALSE){
        log_e("Failed to link shader program\n");
    }

    glValidateProgram(program.id);
    glGetProgramiv(program.id, GL_VALIDATE_STATUS, &linkResult);
    if (linkResult == GL_FALSE) {
        GLsizei log_length = 0;
        GLchar message[1024];
        glGetProgramInfoLog(program.id, sizeof(message), &log_length, message);
        log_e("Error validating shader: %s\n", message);
    }
#endif
}

void ShaderProgram::bind()
{
    glUseProgram(program.id);checkGL();
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
    }
#endif
}
}
