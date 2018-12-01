#include "tests.h"
#include "shader_program.h"

namespace asg{

namespace tests{
std::unique_ptr<Mesh> make_triangle()
{
    static const char* vertexSource = R"(
        attribute vec4 position;
        attribute vec3 color;
        uniform mat4 World;
        varying vec3 vColor;
        void main()
        {
            vColor = color;
          gl_Position = World * vec4(position.xyz, 1.0);
        })";

    static const char* fragmentSource = R"(
        #ifdef GL_ES
            precision mediump float;
        #endif
        varying vec3 vColor;
        void main()
        {
          gl_FragColor = vec4 (vColor, 1.0 );
        })";

    float vertices[] = {0.0f, 0.5f, 0.5f, -0.5f, -0.5f, -0.5f};
    float colors[] = {1.0f, 0.0f, 0.0f
                       , 0.0, 1.0f, 0.0f
                       , 0.0f, 0.0f, 1.0f};
    auto tri = std::make_unique<Mesh>();
    tri->setProgram(std::make_shared<asg::ShaderProgram>(vertexSource, fragmentSource));
    tri->addAttribute(AttribDescr::fromArray("position", vertices, 2));
    tri->addAttribute(AttribDescr::fromArray("color", colors, 3));
    tri->setDrawDescription(DrawDescr{DrawType::Triangles, 3});
    auto rot_u = tri->addUniform("World");
    rot_u.set(glm::mat4(1.0));
    return tri;
}
}

}
