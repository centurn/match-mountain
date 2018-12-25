#pragma once

#include "geo_coords.h"
#include <vector>

namespace geo{

struct Track{
    using Segment = std::vector<Position>;
    using Data = std::vector<Segment>;
    std::string name;
    Data data;
};

Track importGPX(const char* filename);

}
