#include "common.hpp"
#ifdef _DEBUG
#include "error.hpp"
#include "gui/gui.hpp"
#include "gui/gui_util.hpp"
#include "logger.hpp"

// TODO change app namespace
namespace ImGuiProject
{
namespace Gui
{

// private
bool _show_debug_menu_bar = true;
bool _show_demo_window = false;
bool _show_debug_window = false;
int _selected_debug_log_index = -1;
Logger::Log _selected_debug_log;

void drawDebugMenuBar(const ImVec2 viewport_pos)
{
    ImGui::PushFont((int)Font::Debug);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.35f, 0.35f, 0.35f, 0.65f));
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(ImVec2(center.x - 140.0f, viewport_pos.y), ImGuiCond_Always);
    ImGui::Begin("debug_control", nullptr,
        ImGuiWindowFlags_AlwaysAutoResize
        | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoTitleBar);
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 3.0f));
        if (_show_debug_menu_bar)
        {
            if (ImGui::Button("_")) _show_debug_menu_bar = false;
        }
        else
        {
            if (ImGui::Button(">")) _show_debug_menu_bar = true;
        }
        ImGui::MouseCursorToHand();
        ImGui::PopStyleVar();

        if (_show_debug_menu_bar)
        {
            ImGui::Checkbox("demo", &_show_demo_window);
            ImGui::MouseCursorToHand();
            ImGui::SameLine();
            ImGui::Checkbox("debug", &_show_debug_window);
            ImGui::MouseCursorToHand();
        }
    }
    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopFont();
}

void pushDebugStyles() noexcept
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.08f, 0.08f, 0.08f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.08f, 0.08f, 0.08f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.282f, 0.282f, 0.282f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.034f, 0.035f, 0.086f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.034f, 0.035f, 0.086f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, ImVec4(0.034f, 0.035f, 0.086f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.111f, 0.178f, 0.301f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.069f, 0.111f, 0.188f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.069f, 0.111f, 0.188f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Tab, ImVec4(0.09f, 0.09f, 0.12f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TabActive, ImVec4(0.24f, 0.24f, 0.28f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TabHovered, ImVec4(0.24f, 0.24f, 0.28f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.275f, 0.275f, 0.275f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.380f, 0.380f, 0.380f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.427f, 0.427f, 0.427f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TableHeaderBg, ImVec4(0.275f, 0.275f, 0.275f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.965f, 0.965f, 0.965f, 1.0f));
}

void popDebugStyles() noexcept
{
    ImGui::PopStyleColor(17);
    ImGui::PopStyleVar();
}

void drawDebugTabItemGeneral()
{
    if (ImGui::BeginTabItem("General"))
    {
        // TODO add general data

        ImGui::EndTabItem();
    }
}

void drawDebugTabItemLogger()
{
    if (ImGui::BeginTabItem("Logger"))
    {
        ImGui::Text("%d logs", Logger::logs.size());

        ImGui::BeginChild("logger_list", ImVec2(800, 430), false, 0);
        {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 0.0f));
            auto debug_log = Logger::logs;
            for (auto iter = debug_log.begin(); iter != debug_log.end(); ++iter)
            {
                bool is_selected = _selected_debug_log_index == iter->log_id;
                if (ImGui::Selectable(format("%05d %s", iter->log_id, iter->text.c_str()).c_str(), is_selected))
                {
                    _selected_debug_log = *iter;
                    _selected_debug_log_index = iter->log_id;
                }
                ImGui::MouseCursorToHand();
            }

            ImGui::PopStyleVar();
        }
        ImGui::EndChild();

        ImGui::Separator();

        if (_selected_debug_log_index != -1)
        {
            ImGui::BeginChild("logger_detail", ImVec2(800, 70), false, 0);
            {
                ImGui::Text("Log ID %d [%s]", _selected_debug_log.log_id, _selected_debug_log.category.c_str());
                ImGui::Text("%s", _selected_debug_log.timestamp.c_str());
                ImGui::Text("%s (LINE %s)", _selected_debug_log.function.c_str(), _selected_debug_log.line.c_str());
                ImGui::Text("%s", _selected_debug_log.text.c_str());
            }
            ImGui::EndChild();
        }

        ImGui::EndTabItem();
    }
}

void drawDebugWindow(bool* open, const int window_w, const int window_h,
    const State current_state)
{
    pushDebugStyles();

    ImGui::Begin("debug", open,
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoTitleBar);
    {
        ImGui::Text("%dx%d", window_w, window_h);
        ImGui::SameLine();
        ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        ImGui::Separator(); //--------------------------------------------------

        ImGui::Text("%-24s: %d / %d", "has_error / showing?", has_error ? 1 : 0, showing_error_message ? 1 : 0);

        if (0 <= static_cast<int>(current_state)
            && static_cast<int>(current_state) <= static_cast<int>(State::None))
        {
            ImGui::Text("%-24s: [%d]%s", "state",
                current_state, STATE_STR[static_cast<int>(current_state)]);
        }

        State next_state = getNextState();
        if (0 <= static_cast<int>(next_state)
            && static_cast<int>(next_state) <= static_cast<int>(State::None))
        {
            ImGui::Text("%-24s: [%d]%s", "next state",
                next_state, STATE_STR[static_cast<int>(next_state)]);
        }

        if (ImGui::BeginTabBar("DebugTab", ImGuiTabBarFlags_None))
        {
            drawDebugTabItemGeneral();
            drawDebugTabItemLogger();
            ImGui::EndTabBar();
        }
    }
    ImGui::End();

    popDebugStyles();
}

void drawDebugWindows(const int window_w, const int window_h, const State current_state)
{
    ImGui::PushFont((int)Font::Debug);

    if (_show_demo_window)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
        ImGui::ShowDemoWindow(&_show_demo_window);
        ImGui::PopStyleVar();
    }

    if (_show_debug_window)
        drawDebugWindow(&_show_debug_window, window_w, window_h, current_state);

    ImGui::PopFont();
}

} // Gui
} // ImGuiApp
#endif
