#pragma once

#include "mesh.h"

namespace geo{

// Mesh based on heightmap from SRTM data
// Note on axes after import:
// x is aligned along longitude
// y is 'up' (height)
// z is along latitude
class Heightmap{
public:
    Heightmap(const Position& pos);

    void render(const glm::mat4& viewproj, const glm::vec3& light_dir);

    glm::vec3 initialPos() const{
        return initial_eye_pos;
    }

    struct IntersectResult{
        glm::vec3 pos;
        bool success;
    };
    IntersectResult surfacePosition(geo::Position position);

    static constexpr double min_extent = 20*1000.0;// 10 km square around the point of interest

private:
    asg::Mesh terra;
    asg::UniformHandler u_mvp;
    asg::UniformHandler u_light_dir;
    Position origin;// Geo location of origin point
    glm::ivec2 origin_vtx;// Indices of vertex which corresponds to origin point
    glm::ivec2 dims;// Heightmap dimensions
    glm::vec3 initial_eye_pos;
};

}
