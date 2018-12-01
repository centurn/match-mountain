#include "tests.h"
#include "shader_program.h"

namespace asg{

namespace tests{

namespace {
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
}

std::unique_ptr<Mesh> makeTriangle()
{

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

std::unique_ptr<Mesh> makeCube()
{
    float vertices[] = {-0.5f, 0.5f, -0.5
                        , 0.5f, 0.5, -0.5
                        ,-0.5, -0.5, -0.5
                        , 0.5, -0.5, -0.5
                        ,-0.5f, 0.5f, 0.5
                        , 0.5f, 0.5, 0.5
                        ,-0.5, -0.5, 0.5
                        , 0.5, -0.5, 0.5};
    Rushort indices[] = { 2, 1, 0
                      , 1, 2, 3
                      , 1, 3, 5
                      , 5, 3, 7
                      , 3, 2, 6
                      , 3, 6, 7
                      , 2, 0, 6
                      , 6, 0, 4
                      , 0, 1, 4
                      , 4, 1, 5
                      , 4, 5, 6
                      , 6, 5, 7};

    float colors[] =   {1.0f, 0.0f, 0.0f
                       , 0.0, 1.0f, 0.0f
                       , 0.0f, 0.0f, 1.0f
                       , 1.0f, 0.0f, 0.0f
                       , 0.0f, 0.0f, 1.0f
                       , 1.0f, 0.0f, 0.0f
                       , 0.0, 1.0f, 0.0f
                       , 0.0f, 0.0f, 1.0f};
    auto tri = std::make_unique<Mesh>();
    tri->setProgram(std::make_shared<asg::ShaderProgram>(vertexSource, fragmentSource));
    tri->addAttribute(AttribDescr::fromArray("position", vertices, 3));
    tri->addAttribute(AttribDescr::fromArray("color", colors, 3));
    tri->setDrawDescription(DrawDescr{DrawType::Triangles
                                      , sizeof(indices)/2
                                      , std::make_shared<AttribBuffer>(make_span(indices), false)
                                      , ScalarType::UShort});
//    tri->setDrawDescription(DrawDescr{DrawType::Triangles, 3});
    auto rot_u = tri->addUniform("World");
    rot_u.set(glm::mat4(1.0));
    return tri;
}

}

}
