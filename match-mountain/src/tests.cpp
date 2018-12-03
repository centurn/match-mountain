#include "tests.h"
#include "shader_program.h"
#include "glm/vec3.hpp"

namespace asg{

namespace tests{

namespace {

static const char* vs_l = R"(
    attribute vec4 position;
    attribute vec3 color;
    attribute vec3 normal;
    uniform mat4 World;
    uniform mat4 MVP;
    varying vec3 vColor;
    void main()
    {
        vec4 light_dir = normalize(vec4(0., -5.0, 5., 0.));
        float intensity = max(dot(World*vec4(normal, 0.), light_dir), 0.);
        vColor = vec3(1., 1., 1.)*intensity;
      gl_Position = MVP * vec4(position.xyz, 1.0);
    })";

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
    float normals[] = {0.0f, 0.0f, -1.0f
                       , 0.0, 0.0f, -1.0f
                       , 0.0f, 0.0f, -1.0f};
    auto tri = std::make_unique<Mesh>();
    tri->setProgram(std::make_shared<asg::ShaderProgram>(vs_l, fragmentSource));
    tri->addAttribute(AttribDescr::fromArray("position", vertices, 2));
//    tri->addAttribute(AttribDescr::fromArray("color", colors, 3));
    tri->addAttribute(AttribDescr::fromArray("normal", normals, 3));
    tri->setDrawDescription(DrawDescr{DrawType::Triangles, 3});
    auto rot_u = tri->addUniform("World");
    rot_u.set(glm::mat4(1.0));
    return tri;
}

std::unique_ptr<Mesh> makeCube()
{
    vec3 vertices[] = { {-0.5f, 0.5f, -0.5}
                        ,{ 0.5f, 0.5, -0.5}
                        ,{-0.5, -0.5, -0.5}
                        ,{ 0.5, -0.5, -0.5}
                        ,{-0.5f, 0.5f, 0.5}
                        ,{ 0.5f, 0.5, 0.5 }
                        ,{-0.5, -0.5, 0.5 }
                        ,{ 0.5, -0.5, 0.5 }};
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

    vec3 colors[] =   { {1.0f, 0.0f, 0.0f }
                       ,{ 0.0, 1.0f, 0.0f }
                       ,{ 0.0f, 0.0f, 1.0f}
                       ,{ 1.0f, 0.0f, 0.0f}
                       ,{ 0.0f, 0.0f, 1.0f}
                       ,{ 1.0f, 0.0f, 0.0f}
                       ,{ 0.0, 1.0f, 0.0f }
                       ,{ 0.0f, 0.0f, 1.0f} };
    vec3 normals[std::size(colors)];
    static_assert (sizeof(vertices) == sizeof(normals)
                  && sizeof(vertices) == sizeof(colors), "");
    for(auto& i: normals){
        i = vec3(0.f);
    }
    for(size_t j = 0; j < std::size(indices); j += 3){
        auto perpendicular = glm::cross(vertices[indices[j+1]] - vertices[indices[j]]
                ,vertices[indices[j+2]] - vertices[indices[j]]);
        for(auto k = j; k != j+3; ++k){
            normals[indices[k]] += perpendicular;
        }
    }
    for(auto& i: normals){
        i = glm::normalize(i);
    }
    auto tri = std::make_unique<Mesh>();
    tri->setProgram(std::make_shared<asg::ShaderProgram>(vs_l, fragmentSource));
    tri->addAttribute(AttribDescr::fromArray("position", vertices, 3));
//    tri->addAttribute(AttribDescr::fromArray("color", colors, 3));
    tri->addAttribute(AttribDescr::fromArray("normal", normals, 3));
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
