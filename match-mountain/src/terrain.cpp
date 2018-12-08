#include "terrain.h"

#include "import_hgt.h"
#include "shader_program.h"
#include "attrib_buffer.h"

#include "glm/gtc/matrix_transform.hpp"

using namespace asg;
using namespace geo;

namespace  {

static constexpr double min_extent = 10*1000.0;// 10 km square around the point of interest
static const float fov = glm::radians(90.0f);

static const char* vs = R"(
    attribute vec4 position;
    attribute vec3 color;
//    attribute vec3 normal;
  //  uniform mat4 World;
    uniform mat4 MVP;
    varying vec3 vColor;
    void main()
    {
    //    vec4 light_dir = normalize(vec4(0., -5.0, 5., 0.));
      //  float intensity = max(dot(World*vec4(normal, 0.), light_dir), 0.);
//        vColor = vec3(1., 1., 1.)*intensity;
        vColor = color;
      gl_Position = MVP * vec4(position.xyz, 1.0);
    })";

static const char* fs = R"(
    #ifdef GL_ES
        precision mediump float;
    #endif
    varying vec3 vColor;
    void main()
    {
      gl_FragColor = vec4 (vColor, 1.0 );
    })";

struct Vertex{
    vec3 position;
    vec3 color;
    vec3 normal;
};


// convert 'pos' Geographic coords to Scene coords
// Origin is at the start point - we build geometry around it
// x axis is along circle of latitude
// y axis is along meridian
// Units are meters
// Precision is not much high here - it degrages with distance.
// Don't use haversine formula since we are supposed to look from fixed position. And more distant objects need less precision
static inline glm::vec2 groundCoords(Position origin, Position pos) {
    return{ 111'300.0 * glm::cos(pos.lat.value) * (pos.lon.value - origin.lon.value)
          , 111'300.0 * (pos.lat.value - origin.lat.value)};
};

static float eye_height(const ImportHgt& importer, Position pos){
    auto [i, j] = importer.nearestPixel(pos);
    return importer.getPixelHeight(i, j);
}

}

Terrain::Terrain(Position pos)
{
    ImportHgt importer(pos);
    auto rect = importer.getPixelRegion(pos, min_extent);

    eye_pos = vec3(0.0f, eye_height(importer, pos) + 5.0f, 0.0f);

    int j_len = (rect.j_end - rect.j_begin);
    int i_len = (rect.i_end - rect.i_begin);
    size_t  vertex_count = size_t(i_len*j_len);
    std::vector<byte> vb(vertex_count * sizeof(Vertex));
    Vertex* vertices = reinterpret_cast<Vertex*>(vb.data());
    Vertex* cur_vtx = vertices;

    for(int i = rect.i_begin; i != rect.i_end; ++i){
        for(int j = rect.j_begin; j != rect.j_end; ++j, ++cur_vtx){
            float hgt = importer.getPixelHeight(i, j);
            auto grounds = groundCoords(pos, importer.getPixelCoords(i, j));
            new (&cur_vtx->position) vec3(grounds.x, hgt, grounds.y);
            new (&cur_vtx->color) vec3(glm::clamp(hgt/1500, 0.f, 1.f)
                                       , 1.f - glm::clamp(hgt/1500, 0.f, 1.f)
                                       , 0.0f);
        }
    }

    // Generate index buffer
    using IdxType = Ruint;
    size_t indices_count = vertex_count + (vertex_count - 1)*2;
    std::vector<byte> ib(indices_count*sizeof(IdxType));
    IdxType* index = reinterpret_cast<IdxType*>(ib.data());
    for(int i = 0; i != i_len - 1; ++i){
        if(i != 0){// Degenerate triangle when connecting to next row
            *(index++) = Ruint((i + 1)*j_len) - 1;
            *(index++) = Ruint(i*j_len);
        }
        for(int j = 0; j != j_len; ++j){
            *(index++) = Ruint(i*j_len + j);
            *(index++) = Ruint((i + 1)*j_len + j);
        }
    }

    auto vbo = std::make_shared<AttribBuffer>(std::move(vb));
    terra.setProgram(std::make_shared<asg::ShaderProgram>(vs, fs));
    terra.addAttribute(AttribDescr{"position"
                                   , vbo
                                   , 3
                                   , ScalarType::Float
                                   , 0
                                   , sizeof(Vertex)
                       });
    terra.addAttribute(AttribDescr{"color"
                                   , vbo
                                   , 3
                                   , ScalarType::Float
                                   , sizeof (glm::vec3)
                                   , sizeof(Vertex)
                       });
    terra.setDrawDescription(DrawDescr{DrawType::TriangleStrip
                                      , int(indices_count)
                                      , std::make_shared<AttribBuffer>(std::move(ib), false)
                                      , ScalarType::UInt});
    u_mvp = terra.addUniform("MVP");
}

Terrain::~Terrain()
{
}

void Terrain::resize(int w, int h)
{
    AppletBase::resize(w, h);
    projection = glm::perspective(fov
                                      , float(w)/h
                                      , 2.0f
                                      , float(min_extent)*1.5f);
    //glm::mat4 proj = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f);
//    glm::mat4 view = glm::lookAt(eye_pos
//                                 , eye_pos + glm::vec3(0.f, 100.f, 0.f)
//                                 , glm::vec3(0, 1, 0));
}

void Terrain::render()
{
    mat4 viewproj = projection * getCamRotation() * glm::translate(mat4(1), -eye_pos);
    u_mvp.set(viewproj);
    terra.render();
}

void Terrain::mouseMove(glm::ivec2 , glm::vec2 delta, uint pressed_mask)
{
    if(pressed_mask & 1){// Left Mouse Button
        rotation_cam += delta;
    }
}

void Terrain::keyDown(int virtual_keycode)
{
//    log_d("%c", virtual_keycode);
    constexpr float v = 10;
    auto direction = [this](){
        auto cam = glm::inverse(getCamRotation()) *glm::vec4(0, 0, -1, 0);
        return glm::normalize(vec3(cam.x, 0, cam.z));
    };
    switch(virtual_keycode){
    case 'e':
        eye_pos += vec3(0, v, 0);
        break;
    case 'q':
        eye_pos += vec3(0, -v, 0);
        break;
    case 'd':{
        auto dir = direction();
        eye_pos += vec3(-dir.z, 0, dir.x) * v;
        break;
    }
    case 'a':{
        auto dir = direction();
        eye_pos += vec3(dir.z, 0, -dir.x) * v;
        break;
    }
    case 'w':
        eye_pos += direction() * v;
        break;
    case 's':
        eye_pos += -direction() * v;
        break;
    }
}

glm::mat4 Terrain::getCamRotation() const
{
    auto ry = glm::rotate(mat4(1)
                          , fov*(float(-rotation_cam.x*2)/width)
                          , vec3(0,1,0));
    auto rx = glm::rotate(mat4(1)
                          , fov*(float(-rotation_cam.y*2)/width)
                          , vec3(1,0,0));
    return rx*ry;
}

