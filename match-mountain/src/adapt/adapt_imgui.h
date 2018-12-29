#pragma once

#include "asg_base.h"

struct SDL_Window;
namespace asg{class Window;};

namespace adapt{

class Gui{
public:
    explicit Gui(asg::Window* wnd);
    ~Gui();

    void update();

    void render();
private:
    asg::Window* wnd;
};

}
