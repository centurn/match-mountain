#include "terrain.h"

#include "import_hgt.h"
#include "shader_program.h"
#include "attrib_buffer.h"
#include "asg_perf.h"

#include "glm/gtc/matrix_transform.hpp"

using namespace asg;
using namespace geo;

namespace  {

static constexpr double min_extent = 20*1000.0;// 10 km square around the point of interest

static const char* vs = R"(
    attribute vec4 position;
    attribute vec3 color;
    attribute vec3 normal;
    //uniform mat4 World;
    uniform mat4 MVP;
    varying vec3 vColor;
    void main()
    {
        vec3 light_dir = normalize(vec3(0., 5.0, 5.0));
        float intensity = max(dot(normal, light_dir), 0.);
        vColor = color*intensity;
    //    vColor = color;
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
    return{ -111'300.0 * glm::cos(glm::radians(pos.lat.value)) * (pos.lon.value - origin.lon.value)
          , 111'300.0 * (pos.lat.value - origin.lat.value)};
};

static float eye_height(const ImportHgt& importer, Position pos){
    auto [i, j] = importer.nearestPixel(pos);
    return importer.getPixelHeight(i, j);
}

}

struct TerrainData{
    TerrainData(const char* photo_filename);
    asg::BackgroundImage ref_image;
    float initial_vfov;

    asg::Mesh terra;
    asg::UniformHandler u_mvp;
    glm::vec3 initial_eye_pos;
};


TerrainData::TerrainData(const char* photo_filename)
    : ref_image(photo_filename)
{
    Position pos = ref_image.getTexture()->getBitmap().getLocation();
    VALIDATE(pos.isValid());
    initial_vfov = ref_image.getTexture()->getBitmap().getVfov();

    ASG_STOPWATCH("Terrain ctor body");
    ImportHgt importer(pos);
    auto rect = importer.getPixelRegion(pos, min_extent);

    initial_eye_pos = vec3(0.0f, eye_height(importer, pos) + 5.0f, 0.0f);

    int j_len = (rect.j_end - rect.j_begin);
    int i_len = (rect.i_end - rect.i_begin);
    size_t  vertex_count = size_t(i_len*j_len);
    std::vector<byte> vb(vertex_count * sizeof(Vertex));
    Vertex* vertices = reinterpret_cast<Vertex*>(vb.data());
    Vertex* cur_vtx = vertices;

    // Generate mesh geometry
    for(int i = rect.i_begin; i != rect.i_end; ++i){
        for(int j = rect.j_begin; j != rect.j_end; ++j, ++cur_vtx){
            float hgt = importer.getPixelHeight(i, j);
            auto grounds = groundCoords(pos, importer.getPixelCoords(i, j));
            new (&cur_vtx->position) vec3(grounds.x, hgt, grounds.y);
            new (&cur_vtx->color) vec3(glm::clamp(hgt/2000, 0.f, 1.f)
                                       , 1.f - glm::clamp(hgt/2000, 0.f, 1.f)
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

    //Generate normals. TODO: this piece could be generalized and moved to engine part
    for(size_t i = 0; i != vertex_count; ++i){
        vertices[i].normal = vec3(0.f);
    }
    IdxType* indices = reinterpret_cast<IdxType*>(ib.data());
    for(size_t j = 2; j < indices_count; ++j){
        auto perpendicular = glm::cross(vertices[indices[j-1]].position - vertices[indices[j]].position
                ,vertices[indices[j-2]].position - vertices[indices[j]].position);
        if(!(j & 1))
            perpendicular = -perpendicular;
        for(auto k = j-2; k != j+1; ++k){
            vertices[indices[k]].normal += perpendicular;
        }
    }
    for(size_t i = 0; i != vertex_count; ++i){
        assert(glm::length(vertices[i].normal) > 0);
        vertices[i].normal = glm::normalize(vertices[i].normal);
    }

    auto vbo = std::make_shared<AttribBuffer>(std::move(vb));
    terra.setProgram(std::make_shared<asg::ShaderProgram>(vs, fs));
    ASG_STRUCT_ATTRIBUTE(terra, vbo, Vertex, position);
    ASG_STRUCT_ATTRIBUTE(terra, vbo, Vertex, color);
    ASG_STRUCT_ATTRIBUTE(terra, vbo, Vertex, normal);
    terra.setDrawDescription(DrawDescr{DrawType::TriangleStrip
                                      , int(indices_count)
                                      , std::make_shared<AttribBuffer>(std::move(ib), false)
                                      , ScalarType::UInt});
    u_mvp = terra.addUniform("MVP");
}


Terrain::Terrain(const char* image_filename)
    : data(std::make_unique<TerrainData>(image_filename))
{
    vfov = data->initial_vfov;
    eye_pos = data->initial_eye_pos;
}

Terrain::~Terrain()
{
}

void Terrain::resize(glm::ivec2 size)
{
    AppletBase::resize(size);
    data->ref_image.fitScreen(float(width)/height);
}

void Terrain::render()
{
    mat4 projection = glm::perspective(vfov
                                      , float(width)/height
                                      , 2.0f
                                      , float(min_extent)*1.5f);
    mat4 viewproj = projection * getCamRotation() * glm::translate(mat4(1), -eye_pos);
    data->u_mvp.set(viewproj);
    data->terra.render();
    if(ref_image_enabled){
        data->ref_image.render();
    }
}

void Terrain::mouseWheel(glm::ivec2 delta)
{
    vfov *= 1.0f + (-delta.y * 0.02f);
    vfov = glm::clamp(vfov, glm::radians(5.f), glm::radians(120.f));
}

void Terrain::mouseMove(glm::ivec2 , glm::ivec2 delta, uint pressed_mask)
{
    if(pressed_mask & 1){// Left Mouse Button
        rotation_cam += glm::vec2(hfov()*(float(-delta.x)/width)
                             , vfov*(float(-delta.y)/height));
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
    case 'e':// Up
        eye_pos += vec3(0, v, 0);
        break;
    case 'q':// Down
        eye_pos += vec3(0, -v, 0);
        break;
    case 'd':{// Strafe right
        auto dir = direction();
        eye_pos += vec3(-dir.z, 0, dir.x) * v;
        break;
    }
    case 'a':{// Strafe left
        auto dir = direction();
        eye_pos += vec3(dir.z, 0, -dir.x) * v;
        break;
    }
    case 'w':// Forward
        eye_pos += direction() * v;
        break;
    case 's':// Basl
        eye_pos += -direction() * v;
        break;
    case 'i':// Toggle display of reference image
        ref_image_enabled = !ref_image_enabled;
        break;
    case 'o':// Toggle blending of reference image
        ref_image_blend = !ref_image_blend;
        data->ref_image.enableBlending(ref_image_blend);
        break;
    case '\r':// Enter - reset to initial position
        eye_pos = data->initial_eye_pos;
        rotation_cam = {0, 0};
        vfov = data->initial_vfov;
        break;
    }
}

glm::mat4 Terrain::getCamRotation() const
{
    auto ry = glm::rotate(mat4(1)
                          , rotation_cam.x
                          , vec3(0,1,0));
    auto rx = glm::rotate(mat4(1)
                          , rotation_cam.y
                          , vec3(1,0,0));
    return rx*ry;
}

float Terrain::hfov() const
{
    return vfov * width / height;
}

