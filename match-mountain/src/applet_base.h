#pragma once

#include "asg_base.h"

namespace asg{

// The base for individual content micro-'applications'
class AppletBase{
public:
    virtual ~AppletBase();

    // Always arrives before 1st render
    virtual void resize(int w, int h);
    virtual void render() = 0;
protected:
    int width, height;
};

}
