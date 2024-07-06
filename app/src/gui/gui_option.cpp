#include "common.hpp"
#include "image.hpp"
#include "data/internal_setting.hpp"
#include "gui/gui.hpp"
#include "gui/gui_color.hpp"
#include "gui/gui_font.hpp"
#include "midi/connector.hpp"
#include "midi/message_handler.hpp"
#include "model/global.hpp"

namespace StreichfettSse
{
namespace Gui
{

constexpr float ITEM_PADDING_Y = 2.0f;
constexpr float ITEM_SPACING_Y = 6.0f;
constexpr const char* TEXT_CONNECTED = "Connected";
constexpr const char* TEXT_DISCONNECTED = "Disconnected";
constexpr char* TEXT_INFO_NONE = "----";

void drawOptionItem(const char* text)
{
    GuiUtil::PushFont((int)Font::OptionItemBold);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12.0f, ITEM_SPACING_Y));
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ITEM_PADDING_Y);    // text v-align
    ImGui::Text(text);
    ImGui::SameLine(210.0f);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ITEM_PADDING_Y);    // text v-align
    ImGui::PopStyleVar();
    ImGui::PopFont();
}

void beginOptionItemValue()
{
    GuiUtil::PushFont((int)Font::OptionItem);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12.0f, ITEM_SPACING_Y));
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ITEM_PADDING_Y);    // text v-align
    ImGui::PushItemWidth(320.0f);
}

void endOptionItemValue()
{
    ImGui::PopItemWidth();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ITEM_PADDING_Y);    // text v-align
    ImGui::PopStyleVar();
    ImGui::PopFont();
}

template<typename T>
void drawOptionItemValue(const T& v, const bool displayable = false)
{
    beginOptionItemValue();
    if (displayable)
    {
        if constexpr (std::is_same_v<T, Ev>)
        {
            if (v.hasArr())
            {
                ImGui::Text(v.evs());
            }
            else
            {
                ImGui::Text("%d", v.ev());
            }
        }
        else if constexpr (std::is_same_v<T, const char*>)
        {
            ImGui::Text(v);
        }
    }
    else
    {
        ImGui::Text(TEXT_INFO_NONE);
    }
    endOptionItemValue();
}

void drawOptionGroupConnections()
{
    GuiUtil::BeginGroupPanel("Connections", ImVec2(440.0f, 200.0f));
    {
        drawOptionItem("Synth Input Device");
        GuiUtil::PushFont((int)Font::OptionItem);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, ITEM_PADDING_Y));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12.0f, ITEM_SPACING_Y));
        ImGui::PushItemWidth(220.0f);
        if (ImGui::BeginCombo("##SYNTH IN", Connector::synth_input.getPortName().c_str()))
        {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12.0f, 8.0f));
            const auto in_name_list_size = Connector::in_name_list.size();
            for (int n = 0; n < in_name_list_size; ++n)
            {
                const auto is_selected = n == Connector::synth_input.getPortIndex();
                if (n == Connector::key_input.getPortIndex())
                {
                    GuiUtil::TextColoredU32(UI_COLOR_TEXT_DEV_USED, "[USED] %s", Connector::in_name_list[n].c_str());
                }
                else
                {
                    if (ImGui::Selectable(Connector::in_name_list[n].c_str(), is_selected))
                    {
                        reservedFuncs.push_back(std::bind(Connector::openSynthInputPort, n, Connector::in_name_list[n]));
                    }
                    GuiUtil::MouseCursorToHand();
                    if (is_selected) ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::PopStyleVar();
            ImGui::EndCombo();
        }
        GuiUtil::MouseCursorToHand();
        ImGui::PopItemWidth();
        ImGui::SameLine();
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 4.0f));
        if (ImGui::ImageButton((void*)(intptr_t)Image::getTextureId(Image::Texture::Reload), ImVec2(14.0f, 14.0f)))
        {
            reservedFuncs.push_back(std::bind(Connector::resetAllConnections));
        }
        GuiUtil::MouseCursorToHand();
        ImGui::PopStyleVar(3);
        ImGui::PopFont();

        drawOptionItem("Synth Output Device");
        GuiUtil::PushFont((int)Font::OptionItem);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, ITEM_PADDING_Y));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12.0f, ITEM_SPACING_Y));
        ImGui::PushItemWidth(220.0f);
        if (ImGui::BeginCombo("##SYNTH OUT", Connector::synth_output.getPortName().c_str()))
        {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12.0f, 8.0f));
            const auto out_name_list_size = Connector::out_name_list.size();
            for (auto n = 0; n < out_name_list_size; ++n)
            {
                const auto is_selected = n == Connector::synth_output.getPortIndex();
                if (ImGui::Selectable(Connector::out_name_list[n].c_str(), is_selected))
                {
                    reservedFuncs.push_back(std::bind(Connector::openSynthOutputPort, n, Connector::out_name_list[n]));
                }
                GuiUtil::MouseCursorToHand();
                if (is_selected) ImGui::SetItemDefaultFocus();
            }
            ImGui::PopStyleVar();
            ImGui::EndCombo();
        }
        GuiUtil::MouseCursorToHand();
        ImGui::PopItemWidth();
        ImGui::PopStyleVar(2);
        ImGui::PopFont();

        drawOptionItem("Keyboard Input Device");
        GuiUtil::PushFont((int)Font::OptionItem);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, ITEM_PADDING_Y));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12.0f, ITEM_SPACING_Y));
        ImGui::PushItemWidth(220.0f);
        if (ImGui::BeginCombo("##KEY IN", Connector::key_input.getPortName().c_str()))
        {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12.0f, 8.0f));
            const auto in_name_list_size = Connector::in_name_list.size();
            for (int n = 0; n < in_name_list_size; ++n)
            {
                const auto is_selected = n == Connector::key_input.getPortIndex();
                if (n == Connector::synth_input.getPortIndex())
                {
                    GuiUtil::TextColoredU32(UI_COLOR_TEXT_DEV_USED, "[USED] %s", Connector::in_name_list[n].c_str());
                }
                else
                {
                    if (ImGui::Selectable(Connector::in_name_list[n].c_str(), is_selected))
                    {
                        reservedFuncs.push_back(std::bind(Connector::openKeyInputPort, n, Connector::in_name_list[n]));
                    }
                    GuiUtil::MouseCursorToHand();
                    if (is_selected) ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::PopStyleVar();
            ImGui::EndCombo();
        }
        GuiUtil::MouseCursorToHand();
        ImGui::PopItemWidth();
        ImGui::PopStyleVar(2);
        ImGui::PopFont();

        drawOptionItem("Force Adjust Kb MIDI Channel");
        GuiUtil::PushFont((int)Font::OptionItem);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12.0f, ITEM_SPACING_Y));
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, ITEM_PADDING_Y));
        ImGui::Checkbox("##FORCE MIDI CH", &Connector::force_adjust_midi_channel);
        GuiUtil::MouseCursorToHand();
        ImGui::PopStyleVar(2);
        ImGui::PopFont();

        drawOptionItem("SysEx Delay");
        GuiUtil::PushFont((int)Font::OptionItem);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12.0f, ITEM_SPACING_Y));
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, ITEM_PADDING_Y));
        ImGui::PushItemWidth(160.0f);
        ImGui::SliderInt("##SysEx Delay", &Connector::store_delay_duration, 200, 500);
        ImGui::PopItemWidth();
        GuiUtil::MouseCursorToHand();
        ImGui::PopStyleVar(2);
        ImGui::PopFont();
    }
    GuiUtil::EndGroupPanel();
}

// DSI: Streichfett
void drawOptionGroupDeviceSettingInfo()
{
    GuiUtil::BeginGroupPanel("Device Setting Info", ImVec2(320.0f, 200.0f));
    {
        const auto displayable_synth_info = Connector::isSynthConnected();

        drawOptionItem("Synth Device");
        ImGui::PushStyleColor(ImGuiCol_Text, displayable_synth_info ? UI_COLOR_TEXT_CONNECTED : UI_COLOR_TEXT_DISCONNECTED);
        if (displayable_synth_info)
        {
            drawOptionItemValue(TEXT_CONNECTED, true);
        }
        else
        {
            drawOptionItemValue(TEXT_DISCONNECTED, true);
        }
        ImGui::PopStyleColor();

        drawOptionItem("Firmware Version");
        drawOptionItemValue(MessageHandler::inquiry_dump.firmware_version.c_str(), displayable_synth_info && MessageHandler::inquiry_dump.received);

        const auto global = InternalSetting::getGlobalData();
        drawOptionItem("Device ID");
        drawOptionItemValue(global->device_id, displayable_synth_info);
        drawOptionItem("MIDI Channel");
        drawOptionItemValue(global->midi_channel, displayable_synth_info);
        drawOptionItem("Tuning");
        drawOptionItemValue(global->tuning, displayable_synth_info);
        drawOptionItem("Transpose");
        drawOptionItemValue(global->transpose, displayable_synth_info);
        drawOptionItem("Pitch Bend Range");
        drawOptionItemValue(global->pitch_bend_range, displayable_synth_info);
    }
    GuiUtil::EndGroupPanel();
}

void drawOptionPanel()
{
    GuiUtil::PushFont((int)Font::Section);
    drawOptionGroupConnections();
    drawOptionGroupDeviceSettingInfo();
    ImGui::PopFont();
}

} // Gui
} // StreichfettSse