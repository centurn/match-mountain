#include "bitmap.h"

#include "asg_base.h"
#include "asg_storage.h"

#define STBI_FAILURE_USERMSG
#define STBI_ONLY_JPEG
#define STBI_NO_STDIO
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace asg{

Bitmap::Bitmap(const char *source)
{
    log_i("Loading  texture: %s\n", source);

    auto buff = readWholeFile(source);
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
