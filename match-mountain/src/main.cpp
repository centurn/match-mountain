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
using glm::ivec2;

static std::function<void()> loop;
void main_loop() { loop(); }

int main(int /*argc*/, char */*argv*/[])
{
    Window window;

    auto rdr = SDL_CreateRenderer(
        window.getNativeWindow()
        , -1
        , SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_PRESENTVSYNC);

    Terrain app(PICS_DIR "37800 IMG_2844.jpg");
//    Terrain app(PICS_DIR "70000 IMG_0326.jpg");
    //tests::CubeTest app;
    window.setApplet(&app);
    app.resize(window.getSize());

#ifdef __EMSCRIPTEN__
    // Emscrpten does not channel resize event to SDL.
    // Looks like the onl way to handle resize in browser is to subscribe to them this way...
    auto emscCanvasSizeChanged = [](int eventType, const void* reserved, void* userData) -> EM_BOOL {
        ivec2 new_size;
        emscripten_get_canvas_element_size("#canvas", &new_size.x, &new_size.y);
        Window* wnd = reinterpret_cast<Window*>(userData);
        wnd->handleResize(new_size);
        return true;
    };

    EmscriptenFullscreenStrategy fsStrategy = { };
    fsStrategy.scaleMode = EMSCRIPTEN_FULLSCREEN_SCALE_DEFAULT;
    fsStrategy.canvasResolutionScaleMode = EMSCRIPTEN_FULLSCREEN_CANVAS_SCALE_HIDEF;
    fsStrategy.filteringMode = EMSCRIPTEN_FULLSCREEN_FILTERING_NEAREST;
    fsStrategy.canvasResizedCallback = emscCanvasSizeChanged;
    fsStrategy.canvasResizedCallbackUserData = &window;
    emscripten_enter_soft_fullscreen(nullptr, &fsStrategy);

    // TODO: actual handling of contex lost/restoring events
    emscripten_set_webglcontextlost_callback(nullptr, nullptr, true, [](int, const void *, void *userData) -> EM_BOOL{
        log_i("GL Context Lost\n");
        return true;
    });
    emscripten_set_webglcontextrestored_callback(nullptr, nullptr, true, [](int, const void *, void *userData) -> EM_BOOL{
        log_i("GL Context Restored\n");
        return true;
    });
#endif

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
            case SDL_WINDOWEVENT:
                if(e.window.event == SDL_WINDOWEVENT_RESIZED || e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED){
                    window.handleResize({e.window.data1, e.window.data2});
                }
                break;
            case SDL_MOUSEMOTION:
                app.mouseMove({e.motion.x, e.motion.y}
                            , {e.motion.xrel, e.motion.yrel}, e.motion.state);
                break;
            case SDL_MOUSEWHEEL:
            #ifdef __EMSCRIPTEN__// WTF in Chrome the wheel events are coming with 30x larger values than native or Firefox
                app.mouseWheel({glm::clamp(e.wheel.x, -1, 1), glm::clamp(e.wheel.y, -1, 1)});
            #else
                app.mouseWheel({e.wheel.x, e.wheel.y});
            #endif
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
