#include "applet_base.h"

namespace asg{

AppletBase::~AppletBase()
{
}

void AppletBase::resize(int w, int h)
{
    width = w;
    height = h;
}

}
