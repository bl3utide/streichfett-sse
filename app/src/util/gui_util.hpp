#pragma once

namespace GuiUtil
{

template<typename... Args>
bool ImGuiHiddenLabel(bool func(const char*, Args...), int repeat_idx,
    const char* label, float width, Args... args)
{
    ImGui::SetNextItemWidth(width);
    return func(StringUtil::format("##%s_%d", label, repeat_idx).c_str(), args...);
}

/*
    func: function of ImGui widget
    repeat_idx: sequence of the label id
    label: display label
    offset: label text width
    width: control width
*/
template<typename... Args>
bool ImGuiLeftLabel(bool func(const char*, Args...), int repeat_idx,
    const char* label, bool hide_label, float offset, float width,
    bool param_changed, ImU32 param_changed_color, Args... args)
{
    if (param_changed) ImGui::PushStyleColor(ImGuiCol_Text, param_changed_color);
    if (!hide_label)
    {
        auto padding_y = ImGui::GetStyle().FramePadding.y;
        auto y = ImGui::GetCursorPosY();
        ImGui::SetCursorPosY(y + padding_y);
        ImGui::TextUnformatted(label);
        ImGui::SameLine(offset);
        ImGui::SetCursorPosY(y);
    }
    if (param_changed) ImGui::PopStyleColor();
    return ImGuiHiddenLabel(func, repeat_idx, label, width, args...);
}

enum class ImGuiCustomKey : int
{
    Up,
    Left,
    Down,
    Right,
    _COUNT_,
};

void MouseCursorToHand() noexcept;
void TextColoredU32(ImU32 col, const char* fmt, ...) noexcept;
void BeginGroupPanel(const char* name, const ImVec2& size);
void EndGroupPanel();
void PushFont(int font_id) noexcept;
bool IsCustomKeyPressed(ImGuiCustomKey key, bool repeat = true) noexcept;

} // GuiUtil
