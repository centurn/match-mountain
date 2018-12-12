#include "asg_storage.h"
#include <fstream>

namespace asg{
std::vector<uint8_t> readWholeFile(const char* filename){
    log_d("Reading file: %s", filename);
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    VALIDATE(file.good());
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> result(static_cast<size_t>(size));
    if (!file.read(reinterpret_cast<char*>(result.data())
                  , size))
    {
        FAIL();
    }
    return result;
}
}
