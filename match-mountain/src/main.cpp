#include <cstdio>
#include <functional>
#include <cmath>

#include "window.h"
#include "tests.h"
#include "terrain.h"
#include "geo_coords.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#else
#endif

#include <SDL.h>

#include "asg_gl.h"

using namespace asg;

static std::function<void()> loop;
void main_loop() { loop(); }

int main(int /*argc*/, char */*argv*/[])
{
#ifdef __EMSCRIPTEN__
    EmscriptenFullscreenStrategy fsStrategy = { };
    fsStrategy.scaleMode = EMSCRIPTEN_FULLSCREEN_SCALE_DEFAULT;
    fsStrategy.canvasResolutionScaleMode = EMSCRIPTEN_FULLSCREEN_CANVAS_SCALE_HIDEF;
    fsStrategy.filteringMode = EMSCRIPTEN_FULLSCREEN_FILTERING_NEAREST;
    fsStrategy.canvasResizedCallback = nullptr;
    fsStrategy.canvasResizedCallbackUserData = nullptr;
    emscripten_enter_soft_fullscreen(nullptr, &fsStrategy);
#endif

    Window window;

    auto rdr = SDL_CreateRenderer(
        window.getNativeWindow()
        , -1
        , SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_PRESENTVSYNC);

    //geo::Position pos{{37.618758}, {-3.747283}};
//    geo::Position pos{{46.512088}, {11.232680}};

    geo::Position pos{{46.521945}, {11.228202}};
    Terrain app(pos);
    app.resize(window.getWidth(), window.getHeight());

    bool quit = false;

    glFrontFace( GL_CCW );
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    loop = [&]
    {
        SDL_Event e;
        while(SDL_PollEvent(&e))
        {
            switch(e.type){
            case SDL_QUIT:
                quit = true;
                return;
            case SDL_MOUSEMOTION:
                app.mouseMove({e.motion.x, e.motion.y}
                            , {e.motion.xrel, e.motion.yrel}, e.motion.state);
                break;
            case SDL_MOUSEWHEEL:
                app.mouseWheel({e.wheel.x, e.wheel.y});
                break;
            case SDL_KEYDOWN:
                app.keyDown(e.key.keysym.sym);
                break;
            case SDL_KEYUP:
                app.keyUp(e.key.keysym.sym);
                break;
            }
        }

        // Clear the screen
        glClearColor(0.0f, 0.3f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        app.render();

        SDL_GL_SwapWindow(window.getNativeWindow());
    };

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(main_loop, 0, true);
#else
    while(!quit)
        main_loop();
#endif

    SDL_DestroyRenderer(rdr);

    return 0;
}
