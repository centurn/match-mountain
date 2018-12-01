#include "background_image.h"

#include "bitmap.h"
#include "mesh.h"

// Shader sources
static const char* vs_src = R"(
    attribute vec4 position;
    attribute vec2 texcoord;
    varying vec2 vTexCoord;
    void main()
    {
        vTexCoord = texcoord;
        gl_Position = vec4(position.xy, -1., 1.0);
    })";
static const char* fs_src = R"(
    #ifdef GL_ES
        precision mediump float;
    #endif
    varying vec2 vTexCoord;
    uniform sampler2D uTexture;
    void main()
    {
        gl_FragColor = texture2D(uTexture, vTexCoord);
//        gl_FragColor = texture2D(uTexture, vec2(0.5, 0.5));
       //gl_FragColor = vec4 (1.0, 1.0, 1.0, 1.0 );
    })";

namespace asg{

namespace{
static float vertices[] = {-1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f};
static float tex_coords[] = {0.0f, 1.0f,  1.0f,  1.0,   0.0f, 0.0f, 1.0f, 0.0f};
static Ruint indices[] = {0, 1, 2, 2, 1, 3};
//static Rushort indices[] = {0, 2, 1, 1, 2, 3};
}

BackgroundImage::BackgroundImage(const char *src)
{
    mesh.setProgram(std::make_shared<asg::ShaderProgram>(vs_src, fs_src));
    mesh.addAttribute(AttribDescr::fromArray("position", vertices, 2));
    mesh.addAttribute(AttribDescr::fromArray("texcoord", tex_coords, 2));
    mesh.setTexture("uTexture", std::make_shared<Texture>(src));
    mesh.setDrawDescription(DrawDescr{DrawType::Triangles
                                      , 6
                                      , std::make_shared<AttribBuffer>(make_span(indices), false)
                                      , ScalarType::UInt});
}

BackgroundImage::~BackgroundImage()
{
}

void BackgroundImage::render()
{
    mesh.render();
}


}// namespace
