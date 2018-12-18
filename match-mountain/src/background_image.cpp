#include "background_image.h"

#include "bitmap.h"
#include "mesh.h"
#include "asg_gl.h"
#include "glm/gtc/matrix_transform.hpp"

// Shader sources
static const char* vs_src = R"(
    attribute vec4 position;
    attribute vec2 texcoord;
    varying vec2 vTexCoord;
    uniform mat4 uTransform;
    void main()
    {
        vTexCoord = texcoord;
        gl_Position = vec4((uTransform*position).xy, -1., 1.0);
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
    texture = std::make_shared<Texture>(src);
    mesh.setProgram(std::make_shared<asg::ShaderProgram>(vs_src, fs_src));

    std::array<vec2, std::size(vertices)> pos;
    std::copy(std::begin(vertices), std::end(vertices), std::begin(pos));
    float texture_aspect = float(texture->getBitmap().getWidth()) / texture->getBitmap().getHeight();
    int stretch_axis = texture_aspect < 1.f;// Stretch along bigger dimension - preserve picture aspect
    for(auto& i: pos){
        i[stretch_axis] *= texture_aspect;
    }
    mesh.addAttribute(AttribDescr::fromArray("position", pos));

    mesh.addAttribute(AttribDescr::fromArray("texcoord", tex_coords));
    mesh.setTexture("uTexture", texture);
    mesh.setDrawDescription(DrawDescr{DrawType::Triangles
                                      , 6
                                      , std::make_shared<AttribBuffer>(make_span(indices), false)
                                      , ScalarType::UInt});
    u_apha = mesh.addUniform("alpha");
    u_transform = mesh.addUniform("uTransform");
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

    auto transform = proj*glm::rotate(mat4(1.f), rotation, glm::vec3(0, 0, 1));
    u_transform.set(transform);
    mesh.render();

    if(alpha_enabled){
        glDisable (GL_BLEND);
    }
    glDepthMask(GL_TRUE);
}

void BackgroundImage::fitScreen(float screen_aspect)
{
    // Stretch NDC of mesh to counter the stretch of the screen
    proj = {(screen_aspect > 1.f? 1/screen_aspect: 1), 0, 0, 0
             , 0, (screen_aspect < 1.f? screen_aspect: 1), 0, 0
             , 0, 0, 1, 0
             , 0, 0, 0, 1
            };
}


}// namespace
