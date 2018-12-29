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

#include "imgui.h"
#include "imgui_impl_sdl_em.h"

static bool show_demo_window = true;
static bool show_another_window = false;
static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
struct ImguiWrap{
    SDL_Window* window;
    ImguiWrap(SDL_Window* window)
        : window(window)
    {
        // Setup ImGui binding
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        ImGui_ImplSdlGL3_Init(window);
        //io.NavFlags |= ImGuiNavFlags_EnableKeyboard;

        // Setup style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsClassic();

        // Load Fonts
        // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
        // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
        // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
        // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
        // - Read 'misc/fonts/README.txt' for more instructions and details.
        // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
        //io.Fonts->AddFontDefault();
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
        //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
        //IM_ASSERT(font != NULL);
    }
    ~ImguiWrap(){
        ImGui_ImplSdlGL3_Shutdown();
        ImGui::DestroyContext();
    }

    void update(){
        ImGui_ImplSdlGL3_NewFrame(window);
        ImGui::SetNextWindowPos(ImVec2(300, 500), ImGuiCond_FirstUseEver);
        {
            ImGui::Begin("Debug");
            static float f = 0.0f;
            static int counter = 0;
            ImGui::Text("Hello, world!");                           // Display some text (you can use a format string too)
            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our windows open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (NB: most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

    // 2. Show another simple window. In most cases you will use an explicit Begin/End pair to name your windows.
    if (show_another_window)
    {
        ImGui::Begin("Another Window", &show_another_window);
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            show_another_window = false;
        ImGui::End();
    }

    // 3. Show the ImGui demo window. Most of the sample code is in ImGui::ShowDemoWindow(). Read its code to learn more about Dear ImGui!
    if (show_demo_window)
    {
        // small hack to overwrite the hardcoded position of the demo window
        static bool firstTime = true;
        if (firstTime) {
            ImGui::SetNextWindowPos(ImVec2(20, 50), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);
            ImGui::Begin("ImGui Demo", &show_demo_window, ImGuiWindowFlags_MenuBar);
            ImGui::End();
        }

        ImGui::ShowDemoWindow(&show_demo_window);
    }

    }

    void render(){
        ImGui::Render();
    }
};

int main(int /*argc*/, char */*argv*/[])
{
    Window window;

    auto rdr = SDL_CreateRenderer(
        window.getNativeWindow()
        , -1
        , SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_PRESENTVSYNC);

    ImguiWrap gui(window.getNativeWindow());
    Terrain app(PICS_DIR "104600 IMG_0994.jpg");
    app.addTrack(PICS_DIR "_50.gpx");
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

        glBindVertexArray(0);checkGL();
        gui.update();
        app.render();
        glBindVertexArray(0);checkGL();
        gui.render();

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
