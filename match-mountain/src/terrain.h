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

public:
    bool setPosition(geo::Position pos);

private:
    asg::Mesh terra;
    asg::UniformHandler u_mvp;
    glm::mat4 viewproj;
    glm::vec3 eye_pos;
};
