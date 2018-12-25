#pragma once
#include "geo_coords.h"
#include "mesh.h"
#include "heightmap.h"

#include <vector>

namespace geo{

struct Track;

// This class generates Visual representatino of geographic ways
// It needs an instance in order to keep shared parts
class WayGeometryFactory{
public:
    // Create visual representation of single way
    std::unique_ptr<asg::Mesh> createGeometry(const Way& way, const Heightmap* terra);
    asg::MeshVector createGeometry(const geo::Track& track, const Heightmap* terra);

private:
    std::shared_ptr<asg::ShaderProgram> program;
};

}
