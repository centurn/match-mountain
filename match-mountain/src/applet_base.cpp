#include "applet_base.h"

namespace asg{

AppletBase::~AppletBase()
{
}

void AppletBase::resize(glm::ivec2 size)
{
    width = size.x;
    height = size.y;
}

}
