#include "terrain.h"
#include <fstream>

using namespace asg;
using namespace geo;

namespace  {

static constexpr double max_extent = 10*1000.0;// 10 km square around the point of interest

// The following constants define the parameters of .hgt file
// They are for 3-arcsec-per-sample SRTM data
// Changing them may allow to handle higher resolution .hgt, but it wasn't tested
static constexpr int hgt_resolution = 3;// Arcsec per sample in ght file. This code handles only one format
static constexpr int max_hgt_x = 1201;
static constexpr int max_hgt_y = 1201;

std::string makeHgtFilename(Position pos){
    std::string result;
    result += pos.lat.deg() >= 0? 'N' : 'S';
    result += std::to_string(pos.lat.deg());
    result += pos.lon.deg() >= 0? 'E' : 'W';
    result += std::to_string(pos.lon.deg());
    result += ".hgt";
    return result;
}

std::vector<std::byte> readWholeFile(const std::string& filename){
    log_d("Reading file: %s", filename.c_str());
    std::ifstream file(filename.c_str(), std::ios::binary | std::ios::ate);
    VALIDATE(file.good());
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<std::byte> result(static_cast<size_t>(size));
    if (file.read(reinterpret_cast<char*>(result.data())
                  , size))
    {
        return result;
    }
    FAIL();
}

struct Vertex{
    vec3 position;
    vec3 normal;
};

}

Terrain::Terrain(Position pos)
{
    auto hgt = readWholeFile(makeHgtFilename(pos));
    int lat_diff_pixels = int(std::round(max_extent / latSecondApprox)) / hgt_resolution;
    int min_lat_sec = std::min(0, pos.lat.sec()
                               - lat_diff_pixels);
    int max_lat_sec = std::max(max_hgt_y-1, pos.lat.sec()
                               + lat_diff_pixels);
}

Terrain::~Terrain()
{
}

void Terrain::resize(int w, int h)
{

}

void Terrain::render()
{

}

