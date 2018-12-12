#pragma once

#include "geo_coords.h"
#include <vector>

namespace geo{
class ImportHgt{
public:
    ImportHgt(Position pos);

    struct Region{
        int i_begin;
        int j_begin;
        int i_end;
        int j_end;
    };
    Region getPixelRegion(Position center, double min_distance) const;
    Position getPixelCoords(int i, int j) const;
    int getPixelHeight(int i, int j) const;
    std::tuple<int, int> nearestPixel(Position pos) const;
private:
    std::vector<uint8_t> data;
    Position origin;// The north-west corner of the tile
};

}//namespace geo
