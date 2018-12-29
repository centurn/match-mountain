#include "window.h"
#include "asg_gl.h"
#include "applet_base.h"

#include <SDL.h>
#include <cstdlib>

namespace asg {

static constexpr int BITS_PER_CHANNEL = 8;

SDLCoreInit::SDLCoreInit()
{
    log_i("SDL Core init...");
    if (SDL_Init(SDL_INIT_EVERYTHING & ~(SDL_INIT_TIMER | SDL_INIT_HAPTIC)) != 0) {
      PANIC("SDL_Init failed. Error: %s", SDL_GetError());
    }
    log_i("OK\n");
}

SDLCoreInit::~SDLCoreInit()
{
    log_i("SDL Quit\n");
    SDL_Quit();
}

Window::Window()
{
    SDL_DisplayMode mode;
    SDL_GetCurrentDisplayMode(0, &mode);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, BITS_PER_CHANNEL);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, BITS_PER_CHANNEL);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, BITS_PER_CHANNEL);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, BITS_PER_CHANNEL);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, BITS_PER_CHANNEL * 4);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, BITS_PER_CHANNEL * 2);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    uint32_t flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE /*| SDL_WINDOW_FULLSCREEN*/;

    log_i("Creating SDL Window...");
    sdl_window.reset(SDL_CreateWindow("Test!"
                                      , SDL_WINDOWPOS_UNDEFINED
                                      , SDL_WINDOWPOS_UNDEFINED
                                      , mode.w
                                      , mode.h
                                      , flags));
    if (sdl_window == nullptr) {
      PANIC("SDL_CreateWindow Failed. Error: %s", SDL_GetError());
    }
    log_i("OK\n");

    log_i("Creating GL Context...");
    sdl_gl_context.reset(SDL_GL_CreateContext(sdl_window.get()));
    if (sdl_gl_context == nullptr) {
      PANIC("SDL_GL_CreateContext error: %s", SDL_GetError());
    }
    log_i("OK\n");

    SDL_GL_SetSwapInterval(0);
    SDL_GL_GetDrawableSize(sdl_window.get(), &size.x, &size.y);

    log_i("Render target size: %d by %d\n", size.x, size.y);
}

Window::~Window()
{
}

void Window::handleResize(glm::ivec2 new_size)
{
    if(new_size != size){
        log_i("Resize event: %dx%d\n", new_size.x, new_size.y);
        size = new_size;
        glViewport(0,0, size.x, size.y); checkGL();
        app->resize(new_size);
    }
}


void Window::SDL_Window_Deleter::operator()(SDL_Window *window)
{
    log_i("Destroying the SDL window\n");
    SDL_DestroyWindow(window);
}

static_assert(std::is_same<SDL_GLContext, void*>::value, "");
void Window::SDL_Context_Deleter::operator()(SDL_GLContext ctx)
{
    log_i("Destroying the SDL context\n");
    SDL_GL_DeleteContext(ctx);
}

}
