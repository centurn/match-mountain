#include "import_hgt.h"
#include "asg_storage.h"
#include <cstring>// for memcpy
#include <tuple>

using namespace asg;

namespace geo{


namespace{

// The following constants define the parameters of .hgt file
// They are for 3-arcsec-per-sample SRTM data
// Changing them may allow to handle higher resolution .hgt, but it wasn't tested
static constexpr int hgt_resolution = 3;// Arcsec per sample in ght file. This code handles only one format
static constexpr double arc_value_per_px = hgt_resolution/60.0/60.0;
static constexpr int max_hgt_i = 1201;// 'vertical'/latutudal
static constexpr int max_hgt_j = 1201;// 'horizontal'/longitudal


// Form name of .hgt for a given position
static std::string makeHgtFilename(Position pos){
    char buff[12];
    int western = pos.lon.deg() < 0;// Not just a flag - add 1 in western hemisphere
    std::snprintf(buff, sizeof(buff), "%c%02d%c%03d.hgt"
                  , pos.lat.deg() >= 0? 'N' : 'S'
                  , std::abs(pos.lat.deg()%100)
                  , western? 'W': 'E'
                  , std::abs((pos.lon.deg() - western)%1000)) ;
    return SRTM_DIR + std::string(buff);
}

}


ImportHgt::ImportHgt(Position pos)
    : data(readWholeFile(makeHgtFilename(pos).c_str()))
{
    VALIDATE(data.size() == max_hgt_i*max_hgt_j*2);
    origin.lat.value = pos.lat.deg() > 0
            ? pos.lat.deg() + 1
            : pos.lat.deg();
    origin.lon.value = pos.lon.deg() > 0
            ? pos.lon.deg()
            : pos.lon.deg() - 1;
}

ImportHgt::Region ImportHgt::getPixelRegion(Position center, double min_distance) const
{
//    assert(center.lat.value >= origin.lat.value
//           && center.lat.value <= origin.lat.value + 1.0);
//    assert(center.lon.value >= origin.lon.value
//           && center.lon.value <= center.lon.value + 1.0);

    auto center_idx = getNearestPixel(center);

    int lat_diff_pixels = int(std::round(min_distance / latSecApprox)) / hgt_resolution;
    int lon_diff_pixels = int(std::round(min_distance / lonSecApprox(center.lat))) / hgt_resolution;

    auto fit_dim = [](int center, int diff, int max_extent){
        return std::tuple(
            std::max(center - diff, 0)
          , std::min(center + diff, max_extent)
        );
    };
    Region result;
    std::tie((result.top_left.y), result.bot_right.y)
            = fit_dim(center_idx.y, lat_diff_pixels, max_hgt_i);
    std::tie((result.top_left.x), result.bot_right.x)
            = fit_dim(center_idx.x, lon_diff_pixels, max_hgt_j);
    return result;
}

Position ImportHgt::getPixelCoords(glm::ivec2 pt) const
{
    assert(pt.y >= 0 && pt.y <= max_hgt_i && pt.x >= 0 && pt.x <= max_hgt_j);
    return Position{{origin.lat.value + (origin.lat.value >= 0? -pt.y*arc_value_per_px: pt.y*arc_value_per_px)}
               , {origin.lon.value + pt.x*arc_value_per_px} };
}

// Handle endianness in input. .hgt data is signed 16-bit big endian.
static inline int convertPixel(const uint8_t* ptr) {
    // Bit-shifting allows to be intependent on target platform
    uint16_t temp = uint16_t(ptr[0] << 8 | ptr[1]);
    int16_t result;
    // Paranoid version of reinterpretation.
    // The compiler should just generate no code for this line
    std::memcpy(&result, &temp, 2);
    return result;
}

int ImportHgt::getPixelHeight(glm::ivec2 pt) const
{
    assert(pt.y >= 0 && pt.y <= max_hgt_i && pt.x >= 0 && pt.x <= max_hgt_j);
    size_t loc = size_t(pt.y*max_hgt_j + pt.x)*2;
    return convertPixel(&data[loc]);
}

glm::ivec2 ImportHgt::getNearestPixel(Position pos) const
{
    int i = pos.lat.totalSec() / hgt_resolution;
    int j = pos.lon.totalSec() / hgt_resolution;
    return glm::ivec2(j >= 0? j : max_hgt_j + j - 1
                              , i >= 0? max_hgt_i - i - 1: -i);
}

double ImportHgt::arcPerPixel() const
{
    // For now, just return the fixed value
    return arc_value_per_px;
}

}
