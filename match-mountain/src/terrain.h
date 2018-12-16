#pragma once

#include "applet_base.h"
#include "mesh.h"
#include "geo_coords.h"
#include "background_image.h"

struct TerrainData;

class Terrain
    : public asg::AppletBase
{
public:
    Terrain(const char* image_filename);
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
    std::unique_ptr<TerrainData> data;

    glm::vec3 eye_pos;
    float vfov{glm::radians(50.f)};
    glm::vec2 rotation_cam{0,0};// Rotation delta - cumulative in radians

    bool ref_image_enabled = false;
    bool ref_image_blend = true;
};
