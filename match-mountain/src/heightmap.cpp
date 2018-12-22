#include "heightmap.h"

#include "import_hgt.h"
#include "shader_program.h"
#include "asg_perf.h"
#include "glm/gtc/epsilon.hpp"

using namespace asg;
namespace geo{


static const char* vs = R"(
    attribute vec4 position;
    attribute vec3 color;
    attribute vec3 normal;
    //uniform mat4 World;
    uniform mat4 MVP;
    varying vec3 vColor;
    uniform vec3 light_dir;
    void main()
    {
        //vec3 light_dir = normalize(vec3(0., 5.0, 5.0));
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

// convert 'pos' Geographic coords to Scene coords
// Origin is at the start point - we build geometry around it
// x axis is along circle of longitude (east is positive)
// y axis is along meridian (north is positive)
// z is height, positive up (this function returns 0)
// Units are meters
// Precision is not much high here - it degrages with distance.
// Don't use haversine formula since we are supposed to look from fixed position. And more distant objects need less precision
static inline glm::vec3 planarCoords(Position origin, Position pos) {
    return{ 111'300.0 * glm::cos(glm::radians(pos.lat.value)) * (pos.lon.value - origin.lon.value)
          , 111'300.0 * (pos.lat.value - origin.lat.value)
          , 0};
};

static float eye_height(const ImportHgt& importer, Position pos){
    auto idx = importer.getNearestPixel(pos);
    return importer.getPixelHeight(idx);
}

Heightmap::Heightmap(const Position &pos)
{
    ASG_STOPWATCH("Terrain ctor body");
    ImportHgt importer(pos);

    // For convenience of later code, origin is placed on the vertex which is nearest to the photo location
    auto rect = importer.getPixelRegion(pos, min_extent);
    origin_vtx = importer.getNearestPixel(pos);
    origin = importer.getPixelCoords(origin_vtx);
    origin_vtx -= rect.top_left;
    arc_per_pixel = importer.arcPerPixel();

    initial_eye_pos = planarCoords(origin, pos);
    initial_eye_pos.z = eye_height(importer, pos) + 20.0f;

    dims.y = (rect.bot_right.y - rect.top_left.y);
    dims.x = (rect.bot_right.x - rect.top_left.x);
    size_t  vertex_count = size_t(dims.x*dims.y);
    std::vector<byte> vb(vertex_count * sizeof(Vertex));
    Vertex* vertices = reinterpret_cast<Vertex*>(vb.data());
    Vertex* cur_vtx = vertices;

    // Generate mesh geometry
    for(int i = rect.top_left.y; i != rect.bot_right.y; ++i){
        for(int j = rect.top_left.x; j != rect.bot_right.x; ++j, ++cur_vtx){
            auto grounds = planarCoords(origin, importer.getPixelCoords({j, i}));
            float hgt = importer.getPixelHeight(vec2(j, i));
            grounds.z= hgt;
            new (&cur_vtx->position) vec3(grounds);
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
    for(int i = 0; i != dims.y - 1; ++i){
        if(i != 0){// Degenerate triangle when connecting to next row
            *(index++) = Ruint((i + 1) * dims.x) - 1;
            *(index++) = Ruint(i * dims.x);
        }
        for(int j = 0; j != dims.x; ++j){
            *(index++) = Ruint(i*dims.x + j);
            *(index++) = Ruint((i + 1)*dims.x + j);
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
    attribs = reinterpret_cast<const Vertex*>(vbo->data());
    terra.setProgram(std::make_shared<asg::ShaderProgram>(vs, fs));
    ASG_STRUCT_ATTRIBUTE(terra, vbo, Vertex, position);
    ASG_STRUCT_ATTRIBUTE(terra, vbo, Vertex, color);
    ASG_STRUCT_ATTRIBUTE(terra, vbo, Vertex, normal);
    terra.setDrawDescription(DrawDescr{DrawType::TriangleStrip
                                      , int(indices_count)
                                      , std::make_shared<AttribBuffer>(std::move(ib), false)
                                      , ScalarType::UInt});
    u_mvp = terra.addUniform("MVP");
    u_light_dir = terra.addUniform("light_dir");

//    initial_eye_pos = surfaceCoords(pos).pos;
}

void Heightmap::render(const glm::mat4 &viewproj, const glm::vec3 &light_dir)
{
    u_mvp.set(viewproj);
    u_light_dir.set(vec3(light_dir));
    terra.render();
}

IntersectResult intersectTriangle(const vec3& v0, const vec3& v1, const vec3& v2
                                  , const vec3& ray_origin, const vec3& ray_dir)
{
    vec3 surface_normal = cross((v1 - v0), (v2 - v0));
    float den = dot(surface_normal, ray_dir);
    if( glm::epsilonEqual(den, 0.f, 0.00001f))
        return {vec3{}, false};// Ray parallel to the surface
    float surface_d = dot(surface_normal, v0);

    float t = (surface_d - dot(surface_normal, ray_origin)) / den;
    vec3 result = ray_origin + t*ray_dir;

    bool inside = dot(cross((v1 - v0), (result - v0)), surface_normal) > 0
               && dot(cross((v2 - v1), (result - v1)), surface_normal) > 0
               && dot(cross((v0 - v2), (result - v2)), surface_normal) > 0;

    return {result, inside};
}

IntersectResult Heightmap::surfaceCoords(Position position)
{
    // The corner of the cell which is closest to origin
    double delta_lat = position.lat.value - origin.lat.value;
    double delta_lon = position.lon.value - origin.lon.value;
    ivec2 cell = origin_vtx + ivec2{int(delta_lat / arc_per_pixel)
                , int(delta_lon / arc_per_pixel) };
    if(cell.x < 1 || cell.x >= dims.x - 1
            || cell.y < 1 || cell.y >= dims.y - 1){
        return {vec3{}, false};// Position of outside heightmap data
    }

    // Move the index so that it points at the lower-left of the cell of interest
    if(delta_lat < 0)
        --cell.y;
    if(delta_lon < 0)
        --cell.x;
    vec3 ray_origin = planarCoords(origin, position);
    ray_origin.y = 10'000.0f;// Pick from definitely above any mountain
    vec3 ll = getVtx(cell).position;
    vec3 ur = getVtx({cell.x - 1, cell.y + 1}).position;
    assert(ray_origin.x >= ll.x && ray_origin.x <= ur.x
           && ray_origin.z >= ll.z && ray_origin.z <= ur.z);

    vec3 pos_local = ray_origin - ll;// position in local coordinates of cell
    // We know that mesh triangles slice the cell from lower-left to upper right corner
    // So, we can know which one to intersect
    if(pos_local.y > pos_local.x){
        vec3 ul = getVtx({cell.x, cell.y - 1}).position;
        return intersectTriangle(ll, ur, ul, ray_origin, {0, -1, 0});
    }else{
        vec3 lr = getVtx({cell.x+1, cell.y}).position;
        return intersectTriangle(ll, lr, ur, ray_origin, {0, -1, 0});
    }
}

}
