#include "asg_storage.h"
#include <fstream>


namespace asg{
std::vector<uint8_t> readWholeFile(const char* filename){
    log_i("Reading file: %s\n", filename);
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    VALIDATE(file.good());
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> result(static_cast<size_t>(size));
    if (!file.read(reinterpret_cast<char*>(result.data())
                  , size))
    {
        log_e("Failed to load file: %s\n", filename);
        FAIL();
    }
    return result;
}

#ifndef __EMSCRIPTEN__
#include "tinyfiledialogs.h"
std::string fileOpenDialog(const char* default_fn, span<const char *> filters, const char *description)
{
    const char* result =  tinyfd_openFileDialog (
         "Select file to open"// char const * const aTitle , // NULL or ""
       , default_fn// char const * const aDefaultPathAndFile , // NULL or ""
       , int(filters.size())// int const aNumOfFilterPatterns , // 0
       , filters.data()// char const * const * const aFilterPatterns , // NULL {"*.jpg","*.png"}
       , description// char const * const aSingleFilterDescription , // NULL | "image files"
       , 0);// int const aAllowMultipleSelects ) ; // 0

    return result == nullptr? "": result;
}
#else

std::string fileOpenDialog(const char* default_fn, span<const char*> filters, const char* description){
    return "";// Not implemented for browser
}

#endif

}
