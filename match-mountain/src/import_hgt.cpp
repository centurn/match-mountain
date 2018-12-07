#include "import_hgt.h"
#include <fstream>
#include <cstring>// for memcpy

namespace geo{


namespace{

// The following constants define the parameters of .hgt file
// They are for 3-arcsec-per-sample SRTM data
// Changing them may allow to handle higher resolution .hgt, but it wasn't tested
static constexpr int hgt_resolution = 3;// Arcsec per sample in ght file. This code handles only one format
static constexpr double arc_value_per_px = hgt_resolution/60.0/60.0;
static constexpr int max_hgt_i = 1201;// 'vertical'/latutudal
static constexpr int max_hgt_j = 1201;// 'horizontal'/longitudal


static std::string makeHgtFilename(Position pos){
    std::string result;
    result += pos.lat.deg() >= 0? 'N' : 'S';
    result += std::to_string(pos.lat.deg());
    result += pos.lon.deg() >= 0? 'E' : 'W';
    result += std::to_string(pos.lon.deg());
    result += ".hgt";
    return ASSETS_DIR + result;
}

static std::vector<std::byte> readWholeFile(const char* filename){
    log_d("Reading file: %s", filename);
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    VALIDATE(file.good());
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<std::byte> result(static_cast<size_t>(size));
    if (!file.read(reinterpret_cast<char*>(result.data())
                  , size))
    {
        FAIL();
    }
    return result;
}
}


ImportHgt::ImportHgt(Position pos)
    : data(readWholeFile(makeHgtFilename(pos).c_str()))
{
    VALIDATE(data.size() == max_hgt_i*max_hgt_j*2);
    origin.lat.value = pos.lat.deg() > 0
            ? pos.lat.deg() + 1
            : pos.lat.deg();
    origin.lon.value = pos.lon.deg();
}

ImportHgt::Region ImportHgt::getPixelRegion(Position center, double min_distance) const
{
//    assert(center.lat.value >= origin.lat.value
//           && center.lat.value <= origin.lat.value + 1.0);
//    assert(center.lon.value >= origin.lon.value
//           && center.lon.value <= center.lon.value + 1.0);

    auto [center_i, center_j] = nearestPixel(center);

    int lat_diff_pixels = int(std::round(min_distance / latSecApprox)) / hgt_resolution;
    int lon_diff_pixels = int(std::round(min_distance / lonSecApprox(center.lat))) / hgt_resolution;

    auto fit_dim = [](int center, int diff, int max_extent){
        return std::tuple(
            std::max(center - diff, 0)
          , std::min(center + diff, max_extent)
        );
    };
    Region result;
    std::tie((result.i_begin), result.i_end)
            = fit_dim(center_i, lat_diff_pixels, max_hgt_i);
    std::tie((result.j_begin), result.j_end)
            = fit_dim(center_j, lon_diff_pixels, max_hgt_j);
    return result;
}

Position ImportHgt::getPixelCoords(int i, int j) const
{
    assert(i >= 0 && i <= max_hgt_i && j >= 0 && j <= max_hgt_j);
    return Position{{origin.lat.value + (origin.lat.value >= 0? -i*arc_value_per_px: i*arc_value_per_px)}
               , {origin.lon.value + j*arc_value_per_px} };
}

// Handle endianness in input. .hgt data is signed 16-bin big endian.
static inline int convertPixel(const uint8_t* ptr) {
    // Bit-shifting allows to be intependent on target platform
    uint16_t temp = uint16_t(ptr[0] << 8 | ptr[1]);
    int16_t result;
    // Paranoid version of reinterpretation.
    // The compiler should just generate no code for this line
    std::memcpy(&result, &temp, 2);
    return result;
}

int ImportHgt::getPixelHeight(int i, int j) const
{
    assert(i >= 0 && i <= max_hgt_i && j >= 0 && j <= max_hgt_j);
    size_t loc = size_t(i*max_hgt_j + j)*2;
    return convertPixel(reinterpret_cast<const uint8_t*>(&data[loc]));
}

std::tuple<int, int> ImportHgt::nearestPixel(Position pos) const
{
    int i = pos.lat.totalSec() / hgt_resolution;
    return std::make_tuple(pos.lon.value >= 0? max_hgt_i - i: i
                           , pos.lon.totalSec() / hgt_resolution);
}

}
