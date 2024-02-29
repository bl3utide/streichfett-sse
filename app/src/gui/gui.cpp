#include "common.hpp"
#include "error.hpp"
#include "image.hpp"
#include "compressed/arrayed_font.hpp"
#include "data/internal_patch.hpp"
#include "gui/gui.hpp"
#include "gui/gui_color.hpp"
#include "gui/gui_font.hpp"
#include "gui/gui_util.hpp"
#include "midi/connector.hpp"
#ifdef _DEBUG
#include "logger.hpp"
#include "midi/connector_debug.hpp"
#endif

namespace StreichfettSse
{
namespace Gui
{

// private
std::string _app_title;
std::string _app_version;
std::string _app_copyright;
SDL_Window* _window;
SDL_GLContext _gl_context;
const int WINDOW_WIDTH = 720;       // DSI: Streichfett
const int WINDOW_HEIGHT = 560;      // DSI: Streichfett
const float UI_MAIN_CONTENT_WIDTH = WINDOW_WIDTH - 64.0f;
Page _page;
const char* PAGE_STR[static_cast<int>(Page::_COUNT_)] =
{
    "EDIT",
    "OPTION"
};

void setUiStyle() noexcept
{
    ImGuiStyle* style = &ImGui::GetStyle();
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
    style->Colors[ImGuiCol_HeaderHovered] = ImGui::ColorConvertU32ToFloat4(UI_COLOR_HEADER);    // TODO change color? (ref: centro-midi)
    style->Colors[ImGuiCol_HeaderActive] = ImGui::ColorConvertU32ToFloat4(UI_COLOR_HEADER);     // TODO change color? (ref: centro-midi)
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
    style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.0f, 0.00f, 0.43f);                 // TODO change color? (ref: centro-midi)
    style->Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.0f, 0.98f, 0.95f, 0.73f);
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

void drawAboutModal()
{
    ImGuiIO& io = ImGui::GetIO();
    auto modal_window_size = ImVec2(400.0f, 180.0f);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(modal_window_size);
    if (ImGui::BeginPopupModal("about", nullptr,
        ImGuiWindowFlags_AlwaysAutoResize
        | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoTitleBar))
    {
        auto modal_window_pos = ImGui::GetCursorScreenPos();

        ImGui::Indent(50.0f);
        ImGui::PushFont((int)Font::Text);
        ImGui::Dummy(ImVec2(0.0f, 50.0f));

        if (ImGui::BeginTable("about_table", 2))
        {
            ImGui::TableSetupColumn("image", ImGuiTableColumnFlags_WidthFixed, 100.0f);
            ImGui::TableSetupColumn("text", ImGuiTableColumnFlags_WidthStretch);

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Image((void*)(intptr_t)Image::getTextureId(Image::Texture::Icon), ImVec2(80.0f, 80.0f));

            ImGui::TableNextColumn();
            ImGui::Text(_app_title.c_str());
            ImGui::Text("version %s", _app_version.c_str());
            ImGui::Dummy(ImVec2(0.0f, 20.0f));
            ImGui::Text(_app_copyright.c_str());

            ImGui::EndTable();
        }

        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) &&
            io.MousePos.x >= modal_window_pos.x &&
            io.MousePos.x <= modal_window_pos.x + modal_window_size.x &&
            io.MousePos.y >= modal_window_pos.y &&
            io.MousePos.y <= modal_window_pos.y + modal_window_size.y)
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::PopFont();
        ImGui::Unindent(40.0f);

        ImGui::EndPopup();
    }
    ImGui::PopStyleVar();
}

void drawHeader(const int window_width)
{
    ImGui::PushFont((int)Font::Title);
    ImGui::PushStyleColor(ImGuiCol_Text, UI_COLOR_TITLE_TEXT);
    ImGui::Text(_app_title.c_str());
    ImGui::PopStyleColor();
    ImGui::PopFont();

    ImGui::SameLine(0.0f, 12.0f);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 7.0f);
    ImGui::PushFont((int)Font::Version);
    ImGui::PushStyleColor(ImGuiCol_Text, UI_COLOR_VERSION_TEXT);
    ImGui::Text(_app_version.c_str());
    ImGui::PopStyleColor();
    ImGui::PopFont();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 7.0f);

    ImGui::SameLine(window_width - 60.0f, 0.0f);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 6.0f);
    ImGui::PushFont((int)Font::TextBold);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8.0f, 3.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
    if (ImGui::Button("?"))
    {
        ImGui::OpenPopup("about");
    }
    ImGui::PopStyleVar(2);
    ImGui::PopFont();
    ImGui::MouseCursorToHand();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 1.0f);

    drawAboutModal();
}

// DSI: Streichfett
void drawPageSelector()
{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    ImGui::PushFont((int)Font::TextBold);
    ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
    for (int page_i = 0; page_i < static_cast<int>(Page::_COUNT_); ++page_i)
    {
        auto is_current_page = static_cast<int>(_page) == page_i;

        ImGui::PushStyleColor(ImGuiCol_Text, is_current_page ? UI_COLOR_TEXT_PAGE_ACTIVE : UI_COLOR_TEXT_PAGE_INACTIVE);
        if (ImGui::Selectable(PAGE_STR[page_i], is_current_page, 0, ImVec2(80, 20)))
        {
            _page = static_cast<Page>(page_i);

            if (_page == Page::Edit)
                setNextState(State::EnterSoundMode);
            else if (_page == Page::Option)
                setNextState(State::EnterOptionMode);
        }
        if (!is_current_page) ImGui::MouseCursorToHand();
        ImGui::PopStyleColor();

        ImVec2 p0 = ImGui::GetItemRectMin();
        ImVec2 p1 = ImGui::GetItemRectMax();

        draw_list->AddRectFilled(p0, p1, IM_COL32(255, 255, 255, 20));

        if (page_i != static_cast<int>(Page::_COUNT_) - 1) ImGui::SameLine();
    }
    ImGui::PopStyleVar();
    ImGui::PopFont();
}

// DSI: Streichfett
void drawContent()
{
    SoundModel::Patch* current_patch = InternalPatch::getCurrentPatch();
    SoundModel::Patch* original_patch = InternalPatch::getOriginalPatch();

    ImGui::PushStyleColor(ImGuiCol_Text, UI_COLOR_TEXT_BASE);
    if (_page == Page::Edit)    drawEditPanel(current_patch, original_patch);
    if (_page == Page::Option)  drawOptionPanel();
    ImGui::PopStyleColor();
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

void initialize(const std::string& title, const std::string& version, const std::string& copyright)
{
    _app_title = title;
    _app_version = version;
    _app_copyright = copyright;

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
    SDL_DisplayMode current;
    SDL_GetCurrentDisplayMode(0, &current);
    _window = SDL_CreateWindow(_app_title.c_str(),
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

    addAllFonts();
#ifdef _DEBUG
    addAllFontsDebug();
#endif
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
        drawHeader(window_width);

        ImGui::Dummy(ImVec2(10.0f, 0.0f));

        // Beginning of disable screen operation
        bool is_waiting_store_delay = Connector::isWaitingStoreDelay();
#ifdef _DEBUG
        bool is_any_test_sending = Connector::Debug::isAnyTestSending();
#endif

        bool disable_page_content = getState() != State::Idle || is_waiting_store_delay
#ifdef _DEBUG
            || is_any_test_sending
#endif
            ;

        if (disable_page_content) ImGui::BeginDisabled();

        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 32.0f);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 1.0f);
        drawPageSelector();
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 1.0f);

        float y = ImGui::GetCursorPosY();
        ImGui::SetNextWindowPos(ImVec2(vp_pos.x + (window_width - UI_MAIN_CONTENT_WIDTH) * 0.5f, vp_pos.y + y));
#ifdef _DEBUG
        auto use_alt_child_bg = isChildBgAlt();
        if (use_alt_child_bg) ImGui::PushStyleColor(ImGuiCol_ChildBg, DEBUG_UI_COLOR_CHILD_BG);
#endif
        ImGui::BeginChild("main", ImVec2(UI_MAIN_CONTENT_WIDTH, -32.0f), true, 0);
        {
            drawContent();
        }
        ImGui::EndChild();
#ifdef _DEBUG
        if (use_alt_child_bg) ImGui::PopStyleColor();
#endif

        if (disable_page_content) ImGui::EndDisabled();
        // End of disable screen operation

        y = ImGui::GetCursorPosY();
        ImGui::SetNextWindowPos(ImVec2(vp_pos.x + (window_width - UI_MAIN_CONTENT_WIDTH) * 0.5f, vp_pos.y + y));
#ifdef _DEBUG
        if (use_alt_child_bg) ImGui::PushStyleColor(ImGuiCol_ChildBg, DEBUG_UI_COLOR_CHILD_BG);
#endif
        ImGui::BeginChild("annotation", ImVec2(UI_MAIN_CONTENT_WIDTH, 30.0f), true, ImGuiWindowFlags_NoScrollbar);
        {
            ImGui::PushFont((int)Font::TextBold);
            ImGui::PushStyleColor(ImGuiCol_Text, UI_COLOR_ANNOTATION[static_cast<int>(Annotation::getType())]);
            ImGui::Text(Annotation::getText().c_str());
            ImGui::PopStyleColor();
            ImGui::PopFont();
        }
        ImGui::EndChild();
#ifdef _DEBUG
        if (use_alt_child_bg) ImGui::PopStyleColor();
#endif
    }
    ImGui::End();
#ifdef _DEBUG
    drawDebugWindows(window_width, window_height, current_state);
#endif

    postDraw();
}

} // Gui
} // StreichfettSse
