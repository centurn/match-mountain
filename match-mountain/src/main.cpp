#include <cstdio>
#include <functional>
#include <cmath>

#include "glm/mat4x4.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
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
    asg::BackgroundImage background(ASSETS_DIR"37800 IMG_2844.jpg");

    auto tri = tests::makeCube();
    auto rot_u = tri->addUniform("World");

    auto rdr = SDL_CreateRenderer(
        window.getNativeWindow()
        , -1
        , SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_PRESENTVSYNC);

    float angle = 0;
    bool quit = false;

    glFrontFace( GL_CCW );
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glm::mat4 proj = glm::perspective(glm::radians(90.f)
                                      , float(window.getWidth())/window.getHeight()
                                      , 0.1f
                                      , 120.f);
    //glm::mat4 proj = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(0.f, 0.f, 5.f)
                                 , glm::vec3(0.f, 0.f, 0.f)
                                 , glm::vec3(0, 1, 0));

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

        angle += 3.14159f/512;

        // Clear the screen
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        background.render();
        glm::mat4 rot = glm::rotate(glm::mat4(1.0), angle, glm::vec3{0.0, 1.0, -0.3});
        glm::mat4 mt = proj*view*rot;
        rot_u.set(mt);
        tri->render();
        mt = proj*view*glm::translate(rot, glm::vec3(-1.0, 0.5, 0));
        rot_u.set(mt);
        tri->render();
        mt = proj*view*glm::translate(rot, glm::vec3(-2.0, 1.0, 0));
        rot_u.set(mt);
        tri->render();

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
