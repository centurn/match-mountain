#include "bitmap.h"

#define STBI_FAILURE_USERMSG
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "asg_base.h"

namespace asg{

Bitmap::Bitmap(const char *source)
{
    int n;
    data.reset(stbi_load(source, &width, &height, &n, 4));
    if(data == nullptr){
        throw GenericException(stbi_failure_reason());
    }
}

Bitmap::~Bitmap()
{
}

void Bitmap::StbDeleter::operator()(void *data)
{
    stbi_image_free(data);
}

}
