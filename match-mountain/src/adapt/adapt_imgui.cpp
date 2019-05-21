#include "adapt_imgui.h"

#include "imgui.h"
#include "imgui_impl_sdl_em.h"
#include "window.h"

static bool show_demo_window = true;
static bool show_another_window = false;


namespace adapt{

Gui::Gui(asg::Window* wnd)
    : wnd(wnd)
{
    SDL_Window* window = wnd->getNativeWindow();
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
Gui::~Gui(){
    ImGui_ImplSdlGL3_Shutdown();
    ImGui::DestroyContext();
}

void Gui::update(){
    ImGui_ImplSdlGL3_NewFrame(wnd);
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowBgAlpha(0.3f);
    {
        ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_NoTitleBar
                     | ImGuiWindowFlags_NoDecoration
                     | ImGuiWindowFlags_NoMove);
        static float f = 0.0f;
        static int counter = 0;
        ImGui::Text("Hello, world!");                           // Display some text (you can use a format string too)
        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        //ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

        ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our windows open/close state
        ImGui::Checkbox("Another Window", &show_another_window);

        if (ImGui::Button("Button"))                            // Buttons return true when clicked (NB: most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }

//    // 2. Show another simple window. In most cases you will use an explicit Begin/End pair to name your windows.
//    if (show_another_window)
//    {
//        ImGui::Begin("Another Window", &show_another_window);
//        ImGui::Text("Hello from another window!");
//        if (ImGui::Button("Close Me"))
//            show_another_window = false;
//        ImGui::End();
//    }

//    // 3. Show the ImGui demo window. Most of the sample code is in ImGui::ShowDemoWindow(). Read its code to learn more about Dear ImGui!
//    if (show_demo_window)
//    {
//        // small hack to overwrite the hardcoded position of the demo window
//        static bool firstTime = true;
//        if (firstTime) {
//            ImGui::SetNextWindowPos(ImVec2(20, 50), ImGuiCond_FirstUseEver);
//            ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);
//            ImGui::Begin("ImGui Demo", &show_demo_window, ImGuiWindowFlags_MenuBar);
//            ImGui::End();
//        }

//        ImGui::ShowDemoWindow(&show_demo_window);
//    }

}

void Gui::render(){
    ImGui::Render();
}
}
