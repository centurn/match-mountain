#pragma once

#include "applet_base.h"
#include "mesh.h"
#include "geo_coords.h"
#include "background_image.h"
#include "heightmap.h"

struct TerrainData;

class Terrain
    : public asg::AppletBase
{
public:
    Terrain(const char* image_filename);
    virtual ~Terrain() override;

    // Try to load the picture and data for new area.
    // If fails, object is not modified
    void initialize(const char* image_filename);

    void addTrack(const char* filename);

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
    inline glm::vec3 animateLight();

private:
    std::unique_ptr<asg::BackgroundImage> ref_image;
    std::unique_ptr<geo::Heightmap> terra;
    std::unique_ptr<asg::MeshVector> track_geometry;
    asg::UniformHandler u_track_mvp;
    std::string previous_filename;

    float initial_vfov;
    glm::vec3 eye_pos;
    float vfov{glm::radians(50.f)};
    glm::vec2 rotation_cam{0,-glm::radians(90.f)};// Rotation delta - cumulative in radians

    bool ref_image_enabled = false;
    bool ref_image_blend = true;

    float light_rotation = 0.0f;
    bool light_rotation_enabled = false;
};
