#include "common.hpp"
#include "error.hpp"
#include "compressed/arrayed_font.hpp"
#include "gui/gui.hpp"
#include "gui/gui_color.hpp"
#include "gui/gui_util.hpp"
#ifdef _DEBUG
#include "logger.hpp"
#endif

namespace StreichfettSse
{
namespace Gui
{

// private
SDL_Window* _window;
SDL_GLContext _gl_context;
const int WINDOW_WIDTH = 1024;
const int WINDOW_HEIGHT = 768;

void initFonts()
{
    ImFont* font;

    // TITLE
    font = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(
        ArrayedFont::TITLE_DATA,
        ArrayedFont::TITLE_SIZE,
        24.0f);
    IM_ASSERT(font);

    // VERSION
    font = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(
        ArrayedFont::SECTION_DATA,
        ArrayedFont::SECTION_SIZE,
        16.0f);
    IM_ASSERT(font);

    // OPTION_ITEM_TEXT
    font = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(
        ArrayedFont::SECTION_DATA,
        ArrayedFont::SECTION_SIZE,
        18.0f);
    IM_ASSERT(font);

    // OPTION_ITEM_TEXT_BOLD
    font = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(
        ArrayedFont::SECTION_BOLD_DATA,
        ArrayedFont::SECTION_BOLD_SIZE,
        18.0f);
    IM_ASSERT(font);

    // SECTION_TEXT
    font = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(
        ArrayedFont::SECTION_DATA,
        ArrayedFont::SECTION_SIZE,
        22.0f);
    IM_ASSERT(font);

    // TEXT
    font = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(
        ArrayedFont::TEXT_DATA,
        ArrayedFont::TEXT_SIZE,
        16.0f);
    IM_ASSERT(font);

    // TEXT_BOLD
    font = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(
        ArrayedFont::TEXT_BOLD_DATA,
        ArrayedFont::TEXT_BOLD_SIZE,
        16.0f);
    IM_ASSERT(font);

#ifdef _DEBUG
    // DEBUG
    font = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(
        ArrayedFont::DEBUG_DATA,
        ArrayedFont::DEBUG_SIZE,
        14.0f);
    IM_ASSERT(font);

    // DEBUG_PROC_HEAD
    font = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(
        ArrayedFont::DEBUG_DATA,
        ArrayedFont::DEBUG_SIZE,
        20.0f);

    // DEBUG_PROC_HEX
    font = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(
        ArrayedFont::DEBUG_DATA,
        ArrayedFont::DEBUG_SIZE,
        18.0f);
    IM_ASSERT(font);
#endif
}

void setUiStyle() noexcept
{
    ImGuiStyle* style = &ImGui::GetStyle();
    /* TODO set app-specific UI styles
    style->WindowPadding = ImVec2(6.0f, 6.0f);
    style->WindowRounding = 0.0f;
    style->WindowBorderSize = 0.0f;
    style->FramePadding = ImVec2(4, 2);
    style->FrameRounding = 0.0f;
    style->FrameBorderSize = 1.0f;
    style->ItemSpacing = ImVec2(12, 2);
    style->ItemInnerSpacing = ImVec2(8, 6);
    style->IndentSpacing = 22.0f;
    style->ScrollbarSize = 15.0f;
    style->ScrollbarRounding = 0.0f;
    style->GrabMinSize = 12.0f;
    style->GrabRounding = 0.0f;
    style->ChildRounding = 0.0f;
    style->TabRounding = 0.0f;

    style->Colors[ImGuiCol_Text] = ImGui::ColorConvertU32ToFloat4(UI_COLOR_TEXT_BASE);
    style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.632f, 0.016f, 0.016f, 1.0f);
    style->Colors[ImGuiCol_WindowBg] = ImGui::ColorConvertU32ToFloat4(UI_COLOR_BACKGROUND);
    style->Colors[ImGuiCol_ChildBg] = ImGui::ColorConvertU32ToFloat4(UI_COLOR_BACKGROUND_CHILD);
    style->Colors[ImGuiCol_PopupBg] = ImGui::ColorConvertU32ToFloat4(UI_COLOR_POPUP_BG);
    style->Colors[ImGuiCol_Border] = ImGui::ColorConvertU32ToFloat4(UI_COLOR_BORDER);
    style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
    style->Colors[ImGuiCol_FrameBg] = ImGui::ColorConvertU32ToFloat4(UI_COLOR_FRAME_BG);
    style->Colors[ImGuiCol_FrameBgHovered] = ImGui::ColorConvertU32ToFloat4(UI_COLOR_FRAME_BG_HOVERED);
    style->Colors[ImGuiCol_FrameBgActive] = ImGui::ColorConvertU32ToFloat4(UI_COLOR_FRAME_BG_ACTIVE);
    style->Colors[ImGuiCol_TitleBg] = ImGui::ColorConvertU32ToFloat4(UI_COLOR_TITLE_BG);
    style->Colors[ImGuiCol_TitleBgCollapsed] = ImGui::ColorConvertU32ToFloat4(UI_COLOR_TITLE_BG);
    style->Colors[ImGuiCol_TitleBgActive] = ImGui::ColorConvertU32ToFloat4(UI_COLOR_TITLE_BG);
    style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.663f, 0.663f, 0.663f, 1.0f);
    style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.693f, 0.693f, 0.693f, 1.0f);
    style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.56f, 0.56f, 0.58f, 1.0f);
    style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.502f, 0.502f, 0.502f, 1.0f);
    style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.424f, 0.424f, 0.424f, 1.0f);
    style->Colors[ImGuiCol_CheckMark] = ImGui::ColorConvertU32ToFloat4(UI_COLOR_SYMBOL);
    style->Colors[ImGuiCol_SliderGrab] = ImGui::ColorConvertU32ToFloat4(UI_COLOR_SLIDER_GRAB);
    style->Colors[ImGuiCol_SliderGrabActive] = ImGui::ColorConvertU32ToFloat4(UI_COLOR_SLIDER_GRAB_ACTIVE);
    style->Colors[ImGuiCol_Button] = ImGui::ColorConvertU32ToFloat4(UI_COLOR_FRAME_BG);
    style->Colors[ImGuiCol_ButtonHovered] = ImGui::ColorConvertU32ToFloat4(UI_COLOR_FRAME_BG_HOVERED);
    style->Colors[ImGuiCol_ButtonActive] = ImGui::ColorConvertU32ToFloat4(UI_COLOR_FRAME_BG_ACTIVE);
    style->Colors[ImGuiCol_Header] = ImGui::ColorConvertU32ToFloat4(UI_COLOR_HEADER);
    style->Colors[ImGuiCol_HeaderHovered] = ImGui::ColorConvertU32ToFloat4(UI_COLOR_HEADER_HOVERED);
    style->Colors[ImGuiCol_HeaderActive] = ImGui::ColorConvertU32ToFloat4(UI_COLOR_HEADER_ACTIVE);
    style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.0f);
    style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.0f);
    style->Colors[ImGuiCol_Tab] = ImGui::ColorConvertU32ToFloat4(UI_COLOR_FRAME_BG);
    style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
    style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.0f, 0.00f, 1.0f);
    style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
    style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.0f, 0.00f, 1.0f);
    style->Colors[ImGuiCol_TableHeaderBg] = ImGui::ColorConvertU32ToFloat4(UI_COLOR_FRAME_BG);
    style->Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.442f, 0.442f, 0.442f, 1.0f);
    style->Colors[ImGuiCol_TableBorderLight] = ImVec4(0.665f, 0.665f, 0.669f, 1.0f);
    style->Colors[ImGuiCol_TextSelectedBg] = ImGui::ColorConvertU32ToFloat4(UI_COLOR_SYMBOL);
    style->Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.0f, 0.98f, 0.95f, 0.73f);
    */
}

void drawErrorModal()
{
    if (has_error)
    {
        if (!showing_error_message)
        {
            ImGui::OpenPopup("app_error");
            showing_error_message = true;
        }

        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal("app_error", &showing_error_message,
            ImGuiWindowFlags_AlwaysAutoResize
            | ImGuiWindowFlags_NoMove
            | ImGuiWindowFlags_NoTitleBar))
        {
            ImGui::PushFont((int)Font::OptionItemBold);
            ImGui::Text("Application error");
            ImGui::PopFont();
            ImGui::Separator();

            ImGui::Dummy(ImVec2(400.0f, 10.0f));

            ImGui::PushFont((int)Font::OptionItem);
            ImGui::TextWrapped(error_message.c_str());

            ImGui::Dummy(ImVec2(400.0f, 10.0f));

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(16.0f, 4.0f));
            if (ImGui::Button("OK"))
            {
                has_error = false;
                showing_error_message = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::MouseCursorToHand();
            ImGui::PopStyleVar();
            ImGui::PopFont();

            ImGui::EndPopup();
        }
    }
}

void preDraw()
{
    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}

void postDraw()
{
    ImGui::Render();
    glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
        SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
    }
    SDL_GL_SwapWindow(_window);
}

void initialize(const char* app_title)
{
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
    SDL_DisplayMode current;
    SDL_GetCurrentDisplayMode(0, &current);
    _window = SDL_CreateWindow(app_title,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);
    SDL_SetWindowMaximumSize(_window, WINDOW_WIDTH, WINDOW_HEIGHT);
    SDL_SetWindowMinimumSize(_window, WINDOW_WIDTH, WINDOW_HEIGHT);
    _gl_context = SDL_GL_CreateContext(_window);
    SDL_GL_MakeCurrent(_window, _gl_context);
    SDL_GL_SetSwapInterval(1);  // Enable vsync

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.IniFilename = nullptr;

    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.ConfigViewportsNoAutoMerge = false;
    io.ConfigViewportsNoDecoration = false;
    io.ConfigViewportsNoDefaultParent = false;
    io.ConfigViewportsNoTaskBarIcon = true;

    ImGui_ImplSDL2_InitForOpenGL(_window, _gl_context);
    ImGui_ImplOpenGL2_Init();

    initFonts();
    setUiStyle();
}

void finalize() noexcept
{
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_GL_DeleteContext(_gl_context);
    SDL_DestroyWindow(_window);
}

void drawGui()
{
    State current_state = getState();

    preDraw();

    drawErrorModal();

    int window_width, window_height;
    SDL_GetWindowSize(_window, &window_width, &window_height);

    auto vp_pos = ImGui::GetWindowViewport()->WorkPos;
    ImGui::SetNextWindowPos(vp_pos);
    ImGui::SetNextWindowSize(ImVec2((float)window_width, (float)window_height));
    ImGui::Begin("background", nullptr,
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoBringToFrontOnFocus);
    {
#ifdef _DEBUG
        drawDebugMenuBar(vp_pos);
#endif

        // TODO draw everything
    }
    ImGui::End();
#ifdef _DEBUG
    drawDebugWindows(window_width, window_height, current_state);
#endif

    postDraw();
}

} // Gui
} // StreichfettSse
