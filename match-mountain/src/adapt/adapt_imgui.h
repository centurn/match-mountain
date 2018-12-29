#pragma once

#include "asg_base.h"

struct SDL_Window;
namespace adapt{

class Gui{
public:
    explicit Gui(SDL_Window* window);
    ~Gui();

    void update();

    void render();
private:
    SDL_Window* window;
};

}
