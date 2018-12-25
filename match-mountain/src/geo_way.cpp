#include "geo_way.h"

#include "import_gpx.h"
#include "shader_program.h"

using namespace asg;
static constexpr float width = 1.f;

static const char* vs = R"(
    attribute vec3 position;
    uniform mat4 MVP;
//    varying vec3 vColor;
    void main()
    {
      gl_Position = MVP * vec4(position.xyz, 1.0);
    })";

static const char* fs = R"(
    #ifdef GL_ES
        precision mediump float;
    #endif
    //varying vec3 vColor;
    void main()
    {
        vec3 vColor = vec3(0.0, 1.0, 1.0);
        gl_FragColor = vec4 (vColor, 1.0 );
    })";

namespace geo {

std::unique_ptr<asg::Mesh> WayGeometryFactory::createGeometry(const Way &way, const Heightmap* terra)
{
    if(way.size() < 2)
        return nullptr;

    auto path = terra->surfaceCoords(way);
    assert(path.size() >= 2);

    std::vector<vec3> vertices;
    std::vector<Ruint> indices;
    vec3 z_offeset{0, 0, 1.f}; // To avoid z-fighting
    for(size_t i = 1; i != path.size(); ++i ){
        vec3 section = path[i] - path[i-1];
        auto len = glm::length(section);
        vec3 section_dir = section / len;
        // Normal to the section which is parallel to the ground and to the right of the section
        vec3 horiz_norm = cross(section_dir, vec3{0, 0 , 1});
        Ruint base_idx = Ruint(vertices.size());
        vertices.emplace_back(path[i-1] - horiz_norm*width + z_offeset);
        vertices.emplace_back(path[i-1] + horiz_norm*width + z_offeset);
        vertices.emplace_back(path[i] - horiz_norm*width + z_offeset);
        vertices.emplace_back(path[i] + horiz_norm*width + z_offeset);
        indices.push_back(base_idx);
        indices.push_back(base_idx + 1);
        indices.push_back(base_idx + 2);

        indices.push_back(base_idx + 1);
        indices.push_back(base_idx + 3);
        indices.push_back(base_idx + 2);
    }

    auto result = std::make_unique<asg::Mesh>();
    result->addAttribute(AttribDescr{
                              "position"
                             , std::make_shared<AttribBuffer>(asg::make_span(vertices))// TODO: possible move-optimization
                             , 3
                             , ScalarType::Float
                             , 0
                             , 0
                         });
    result->setDrawDescription(asg::DrawDescr{DrawType::Triangles
                                      , Rsize(indices.size())
                                      , std::make_shared<AttribBuffer>(asg::make_span(indices), false)
                                      , ScalarType::UInt});

    if(!program){
        program = std::make_shared<asg::ShaderProgram>(vs, fs);
    }
    result->setProgram(program);
    return result;
}

MeshVector WayGeometryFactory::createGeometry(const Track &track, const Heightmap* terra)
{
    MeshVector result;
    for(const auto& i: track.data){
        auto way = createGeometry(i, terra);
        if(way)
            result.push_back(std::move(way));
    }
    return result;
}

}
