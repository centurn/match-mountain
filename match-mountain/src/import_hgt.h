#pragma once

#include "geo_coords.h"
#include <vector>
#include "glm/vec2.hpp"

namespace geo{

// Importer from SRTM .hgt file format
class ImportHgt{
public:
    ImportHgt(Position pos);

    // Note on axes here: x is aligned along longitude, y - along latitude
    // This does not correspond to axes in generated 3D space
    struct Region{
        glm::ivec2 top_left;
        glm::ivec2 bot_right;
    };
    Region getPixelRegion(Position center, double min_distance) const;
    Position getPixelCoords(glm::ivec2 pt) const;
    int getPixelHeight(glm::ivec2 pt) const;
    glm::ivec2 getNearestPixel(Position pos) const;

    //Resolution of data - in lat or lon per 'pixel'
    double arcPerPixel() const;
private:
    std::vector<uint8_t> data;
    Position origin;// The north-west corner of the tile
};

}//namespace geo
