#pragma once
// Functions to generate some test meshes

#include "mesh.h"
#include "applet_base.h"
#include "background_image.h"

namespace asg{

namespace tests{

std::unique_ptr<Mesh> makeTriangle();

std::unique_ptr<Mesh> makeCube();

class CubeTest
    : public AppletBase
{
public:
    CubeTest();
    virtual ~CubeTest() override;
    virtual void resize(glm::ivec2 size) override;
    virtual void render() override;
private:
    BackgroundImage background;
    std::unique_ptr<Mesh> cube;
    UniformHandler u_world;
    UniformHandler u_mvp;
    mat4 viewproj {1.0f};
    float angle{0};
};

}

}
