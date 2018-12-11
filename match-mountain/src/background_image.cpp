#include "background_image.h"

#include "bitmap.h"
#include "mesh.h"
#include "asg_gl.h"

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
    uniform float alpha;
    void main()
    {
        gl_FragColor = vec4(texture2D(uTexture, vTexCoord).rgb, alpha);
//        gl_FragColor = texture2D(uTexture, vec2(0.5, 0.5));
       //gl_FragColor = vec4 (1.0, 1.0, 1.0, 1.0 );
    })";

namespace asg{

namespace{
static const vec2 vertices[] = {{-1.0f, -1.0f}, {1.0f, -1.0f}, {-1.0f, 1.0f}, {1.0f, 1.0f}};
static const vec2 tex_coords[] = {{0.0f, 1.0f},  {1.0f,  1.0},  {0.0f, 0.0f}, {1.0f, 0.0f}};
static const Ruint indices[] = {0, 1, 2, 2, 1, 3};
}

BackgroundImage::BackgroundImage(const char *src)
{
    auto texture = std::make_shared<Texture>(src);
    texture_aspect = float(texture->getBitmap().getWidth()) / texture->getBitmap().getHeight();
    mesh.setProgram(std::make_shared<asg::ShaderProgram>(vs_src, fs_src));
    auto positions_attr = AttribDescr::fromArray("position", vertices, 2);
    positions = positions_attr.buff;
    mesh.addAttribute(positions_attr);
    mesh.addAttribute(AttribDescr::fromArray("texcoord", tex_coords, 2));
    mesh.setTexture("uTexture", texture);
    mesh.setDrawDescription(DrawDescr{DrawType::Triangles
                                      , 6
                                      , std::make_shared<AttribBuffer>(make_span(indices), false)
                                      , ScalarType::UInt});
    u_apha = mesh.addUniform("alpha");
}

BackgroundImage::~BackgroundImage()
{
}

void BackgroundImage::render()
{
    glDepthMask(GL_FALSE);
    if(alpha_enabled){
        glEnable (GL_BLEND);
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        u_apha.set(0.5f);
    }else{
        u_apha.set(1.0f);
    }
    mesh.render();
    if(alpha_enabled){
        glDisable (GL_BLEND);
    }
    glDepthMask(GL_TRUE);
}

void BackgroundImage::fitScreen(float screen_aspect)
{
    std::array<vec2, std::size(vertices)> pos;
    std::copy(std::begin(vertices), std::end(vertices), std::begin(pos));
    int stretch_axis;
    float stretch_factor;
    if(screen_aspect > texture_aspect){
        stretch_axis = 0;
        stretch_factor = texture_aspect/screen_aspect;
    }else{
        stretch_axis = 1;
        stretch_factor = screen_aspect/texture_aspect;
    }

    for(auto& i: pos){
        i[stretch_axis] *= stretch_factor;
    }
    positions->set(make_span(pos));
    mesh.makeDirty();
}


}// namespace
