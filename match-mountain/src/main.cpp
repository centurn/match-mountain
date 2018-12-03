#include <cstdio>
#include <functional>
#include <cmath>

#include "window.h"
#include "background_image.h"
#include "tests.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#endif

#include <SDL.h>

#include "asg_gl.h"
using namespace asg;

static std::function<void()> loop;
void main_loop() { loop(); }

int main(int /*argc*/, char */*argv*/[])
{
    Window window;

    auto rdr = SDL_CreateRenderer(
        window.getNativeWindow()
        , -1
        , SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_PRESENTVSYNC);

    tests::CubeTest app;
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
            if(e.type == SDL_QUIT){
                quit = true;
                return;
            }
        }

        // Clear the screen
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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
