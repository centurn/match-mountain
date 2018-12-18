#pragma once

#include "mesh.h"

namespace geo{

class Heightmap{
public:
    Heightmap(const Position& pos);

    void render(const glm::mat4& viewproj, const glm::vec3& light_dir);

    glm::vec3 initialPos() const{
        return initial_eye_pos;
    }

    static constexpr double min_extent = 20*1000.0;// 10 km square around the point of interest

private:
    asg::Mesh terra;
    asg::UniformHandler u_mvp;
    asg::UniformHandler u_light_dir;
    int i_len, j_len;// Heightmap dimensions
    glm::vec3 initial_eye_pos;
};

}
