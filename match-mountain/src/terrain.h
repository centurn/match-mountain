#pragma once

#include "applet_base.h"
#include "mesh.h"
#include "geo_coords.h"
#include "background_image.h"

class Terrain
    : public asg::AppletBase
{
public:
    Terrain(geo::Position pos);
    virtual ~Terrain() override;

    // AppletBase interface
public:
    void resize(glm::ivec2 size) override;
    void render() override;
    void mouseMove(glm::ivec2 pos, glm::ivec2 delta, asg::uint pressed_mask) override;
    void mouseWheel(glm::ivec2 delta) override;
    void keyDown(int virtual_keycode) override;

public:
    bool setPosition(geo::Position pos);

private:
    inline glm::mat4 getCamRotation() const;
    inline float hfov() const;

private:
    asg::Mesh terra;
    asg::UniformHandler u_mvp;
    glm::vec3 eye_pos;
    glm::vec3 initial_eye_pos;
    float vfov{glm::radians(20.f)};
    glm::vec2 rotation_cam{0,0};// Rotation delta - cumulative in radians

    asg::BackgroundImage ref_image;
    bool ref_image_enabled = false;
    bool ref_image_blend = true;
};
