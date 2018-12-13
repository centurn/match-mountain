#include "bitmap.h"

#include "asg_base.h"
#include "asg_storage.h"

#define STBI_FAILURE_USERMSG
#define STBI_ONLY_JPEG
#define STBI_NO_STDIO
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "exif.h"

namespace asg{

Bitmap::Bitmap(const char *source)
{
    log_i("Loading  texture: %s\n", source);

    auto buff = readWholeFile(source);
    easyexif::EXIFInfo result;
    result.GeoLocation.Latitude = -1000.0;
    result.GeoLocation.Longitude = -1000.0;
    result.parseFrom(buff.data(), uint(buff.size()));
    // Looks like Easyexif does not touch the fields if data is invalid or unset
    location = {{result.GeoLocation.Latitude}, {result.GeoLocation.Longitude}};

    int ignore;
    data.reset(stbi_load_from_memory(buff.data(), int(buff.size()), &width, &height, &ignore, 4));
    VALIDATE(data);
    log_i("Texture loaded\n");        
}

Bitmap::~Bitmap()
{
}

void Bitmap::StbDeleter::operator()(void *data)
{
    stbi_image_free(data);
}

}
