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

static const float default_fov { glm::radians(45.f)};
static const float default_sensor_height { 14.9f };

static float vfovFromFocal(float sensor_size, float focal_len){
    return 2*glm::atan(sensor_size/(2.f*focal_len));
}

static float fovFromExif(const easyexif::EXIFInfo& info){
    if(info.FocalLengthIn35mm != 0){// That case goes without assumptions
        return vfovFromFocal(24.f, info.FocalLengthIn35mm);
    }
    if(info.FocalLength > 0){
        return vfovFromFocal(default_sensor_height, float(info.FocalLength));
    }
    log_i("No info on focal length in Exif. Using default fov\n");
    return default_fov;
}

Bitmap::Bitmap(const char *source)
{
    log_i("Loading  texture: %s\n", source);

    auto buff = readWholeFile(source);
    easyexif::EXIFInfo exif;
    exif.GeoLocation.Latitude = -1000.0;
    exif.GeoLocation.Longitude = -1000.0;
    exif.FocalLength = 0;
    exif.FocalLengthIn35mm = 0;
    auto parse_result = exif.parseFrom(buff.data(), uint(buff.size()));
    VALIDATE(parse_result == PARSE_EXIF_SUCCESS);

    // Looks like Easyexif does not touch the fields if data is invalid or unset
    location = {{exif.GeoLocation.Latitude}, {exif.GeoLocation.Longitude}};
    vfov = fovFromExif(exif);

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
