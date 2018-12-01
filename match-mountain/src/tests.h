#pragma once
// Functions to generate some test meshes

#include "mesh.h"

namespace asg{

namespace tests{

std::unique_ptr<Mesh> makeTriangle();

std::unique_ptr<Mesh> makeCube();

}

}
