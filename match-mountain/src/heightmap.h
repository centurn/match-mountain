#pragma once

#include "mesh.h"

namespace geo{

struct IntersectResult{
    glm::vec3 pos;
    bool success;
};

// Mesh based on heightmap from SRTM data
// Note on axes after import:
// x axis is along circle of longitude, east is positive
// y axis is along meridian, north is positive
// z is height, positive up
class Heightmap{
public:
    Heightmap(const Position& pos);

    void render(const glm::mat4& viewproj, const glm::vec3& light_dir);

    glm::vec3 initialPos() const{
        return initial_eye_pos;
    }

    IntersectResult surfaceCoords(geo::Position position);

    static constexpr double min_extent = 20*1000.0;// 10 km square around the point of interest

private:
    struct Vertex{
        glm::vec3 position;
        glm::vec3 color;
        glm::vec3 normal;
    };
    inline const Heightmap::Vertex& getVtx(glm::ivec2 idx) const;

    asg::Mesh terra;
    const Vertex* attribs;
    asg::UniformHandler u_mvp;
    asg::UniformHandler u_light_dir;

    glm::vec3 initial_eye_pos;
    Position origin;// Geo location of origin point
    glm::ivec2 origin_vtx;// Indices of vertex which corresponds to origin point
    glm::ivec2 dims;// Heightmap dimensions
    double arc_per_pixel;// Resolution of original data file - and of generated mesh
};

const Heightmap::Vertex& Heightmap::getVtx(glm::ivec2 idx) const
{
    return attribs[idx.y*dims.x + idx.x];
}

}
