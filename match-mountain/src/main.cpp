#include <cstdio>
#include <functional>
#include <cmath>

#include "glm/mat4x4.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "window.h"
#include "background_image.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#endif

#include <SDL.h>

#include "asg_gl.h"
using namespace asg;

// Shader sources
static const GLchar* vertexSource = R"(
    attribute vec4 position;
    attribute vec3 color;
    uniform mat4 World;
    varying vec3 vColor;
    void main()
    {
        vColor = color;
      gl_Position = World * vec4(position.xyz, 1.0);
    })";

static const GLchar* fragmentSource = R"(
    #ifdef GL_ES
        precision mediump float;
    #endif
    varying vec3 vColor;
    void main()
    {
      gl_FragColor = vec4 (vColor, 1.0 );
    })";

namespace {
static std::function<void()> loop;
void main_loop() { loop(); }

GLfloat vertices[] = {0.0f, 0.5f, 0.5f, -0.5f, -0.5f, -0.5f};
GLfloat colors[] = {1.0f, 0.0f, 0.0f
                   , 0.0, 1.0f, 0.0f
                   , 0.0f, 0.0f, 1.0f};
}

int main(int /*argc*/, char */*argv*/[])
{
    Window window;
    asg::BackgroundImage background(ASSETS_DIR"37800 IMG_2844.jpg");

    Mesh tri;
    tri.setProgram(std::make_shared<asg::ShaderProgram>(vertexSource, fragmentSource));
    tri.addAttribute(AttribDescr::fromArray("position", vertices, 2));
    tri.addAttribute(AttribDescr::fromArray("color", colors, 3));
    auto rot_u = tri.addUniform("World");

    auto rdr = SDL_CreateRenderer(
        window.getNativeWindow(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);

    float angle = 0;
    bool quit = false;
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
        glm::mat4 mt = glm::rotate(glm::mat4(1.0), angle, glm::vec3{0.0, 1.0, 0.0});
        rot_u.set(mt);

        // Clear the screen
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        background.render();
        tri.draw();

        glDrawArrays(GL_TRIANGLES, 0, 3);

        SDL_GL_SwapWindow(window.getNativeWindow());
    };

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(main_loop, 0, true);
#else
    while(!quit)
        main_loop();
#endif

    return 0;
}
