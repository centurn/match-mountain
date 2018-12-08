#pragma once

#include "applet_base.h"
#include "mesh.h"
#include "geo_coords.h"

class Terrain
    : public asg::AppletBase
{
public:
    Terrain(geo::Position pos);
    virtual ~Terrain() override;

    // AppletBase interface
public:
    void resize(int w, int h) override;
    void render() override;
    void mouseMove(glm::ivec2 pos, glm::vec2 delta, asg::uint pressed_mask) override;
    void keyDown(int virtual_keycode) override;

public:
    bool setPosition(geo::Position pos);

private:
    asg::Mesh terra;
    asg::UniformHandler u_mvp;
    glm::mat4 projection;
    glm::vec3 eye_pos;
    glm::ivec2 rotation_cam{0,0};// Rotation delta - cumulative in screen coords
};
