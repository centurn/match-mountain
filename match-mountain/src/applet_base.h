#pragma once

#include "asg_base.h"
#include "glm/vec2.hpp"

namespace asg{

enum class MouseButton{
     Left = 1
   , Middle = 2
   , Right = 3
};

// The base for individual content micro-'applications'
class AppletBase{
public:
    virtual ~AppletBase();

    // 1st call to resize() always arrives before 1st render
    virtual void resize(glm::ivec2 size);
    virtual void render() = 0;
//    virtual void mouseDown(int idx, int x, MouseButton btn){}
//    virtual void mouseUp(int idx, int x, MouseButton btn){}
    virtual void mouseMove(glm::ivec2 pos, glm::ivec2 delta, uint pressed_mask){}
    virtual void mouseWheel(glm::ivec2 delta){}

    // Key Pressed/Released events
    // Currently support only virtual keys.
    // Most character key codes correspond to they ascii values
    virtual void keyDown(int virtual_keycode){}
    virtual void keyUp(int virtual_keycode){}
protected:
    int width, height;
};

}
