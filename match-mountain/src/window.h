#pragma once

#include "asg_base.h"
#include <memory>// for unique_ptr
#include "glm/vec2.hpp"

// Forward declarations - to limit includes of SDL.h
struct SDL_Window;

namespace  asg {

class AppletBase;

// Contain the matching init/deinit of SDL core
// Ensures properly closed core in case of error in Window ctor
struct SDLCoreInit{
    SDLCoreInit();
    ~SDLCoreInit();
};

class Window{
public:
    Window();
    ~Window();
    SDL_Window* getNativeWindow(){
        return sdl_window.get();
    }
    glm::ivec2 getSize() const{
        return size;
    }
    void setApplet(AppletBase* applet){
        app = applet;
    }
    void handleResize(glm::ivec2 new_size);
private:
    struct SDL_Window_Deleter{// The best way to feed deleter to unique_ptr?
        void operator()(SDL_Window* window);
    };
    struct SDL_Context_Deleter{
        void operator()(void* ctx);

    };
    SDLCoreInit sdl_core;
    std::unique_ptr<SDL_Window, SDL_Window_Deleter> sdl_window;

    //SDL_GLContext is defined as void*. That's validated in .cpp file
    std::unique_ptr<void, SDL_Context_Deleter> sdl_gl_context;

    glm::ivec2 size;
    AppletBase* app;
};


}

