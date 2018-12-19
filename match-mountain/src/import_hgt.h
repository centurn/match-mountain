#pragma once

#include "geo_coords.h"
#include <vector>
#include "glm/vec2.hpp"

namespace geo{
class ImportHgt{
public:
    ImportHgt(Position pos);

    struct Region{
        glm::ivec2 top_left;
        glm::ivec2 bot_right;
    };
    Region getPixelRegion(Position center, double min_distance) const;
    Position getPixelCoords(int i, int j) const;
    int getPixelHeight(int i, int j) const;
    glm::ivec2 nearestPixel(Position pos) const;
private:
    std::vector<uint8_t> data;
    Position origin;// The north-west corner of the tile
};

}//namespace geo
