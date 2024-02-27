#include "common.hpp"
#include "image.hpp"
#include "data/internal_setting.hpp"
#include "gui/gui.hpp"
#include "gui/gui_color.hpp"
#include "gui/gui_font.hpp"
#include "gui/gui_util.hpp"
#include "midi/connector.hpp"
#include "model/global.hpp"

namespace StreichfettSse
{
namespace Gui
{

const float ITEM_PADDING_Y = 2.0f;
const float ITEM_SPACING_Y = 6.0f;
const char* TEXT_CONNECTED = "Connected";
const char* TEXT_DISCONNECTED = "Disconnected";
const char* TEXT_INFO_NONE = "----";

void drawOptionItem(const char* text)
{
    ImGui::PushFont((int)Font::OptionItemBold);
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
    ImGui::PushFont((int)Font::OptionItem);
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

template<class T>
void drawOptionItemValue(const T& v, const bool displayable = false)
{
    beginOptionItemValue();
    //if constexpr (std::is_same_v<T, int>) // TODO delete fix Global Model
    if (displayable)
    {
        if constexpr (std::is_same_v<T, Ev>)
        {
            if (v.hasArr())
            {
                //ImGui::Text("%d", v); // TODO delete Global Model fix
                ImGui::Text(v.evs());
            }
            //else if constexpr (std::is_same_v<T, const char*>)    // TODO delete fix Global Model
            else
            {
                //ImGui::Text(v);   // TODO delete fix Global Model
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
    ImGui::BeginGroupPanel("Connections", ImVec2(440.0f, 200.0f));
    {
        drawOptionItem("Synth Input Device");
        ImGui::PushFont((int)Font::OptionItem);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, ITEM_PADDING_Y));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12.0f, ITEM_SPACING_Y));
        ImGui::PushItemWidth(220.0f);
        if (ImGui::BeginCombo("##SYNTH IN", Connector::synth_conn.input_port_name.c_str()))
        {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12.0f, 8.0f));
            for (int n = 0; n < Connector::in_name_list.size(); ++n)
            {
                bool is_selected = n == Connector::synth_conn.input_port_index;
                if (n == Connector::key_conn.input_port_index)
                {
                    ImGui::TextColoredU32(UI_COLOR_TEXT_DEV_USED, "[USED] %s", Connector::in_name_list[n].c_str());
                }
                else
                {
                    if (ImGui::Selectable(Connector::in_name_list[n].c_str(), is_selected))
                    {
                        Connector::synth_conn.input_port_index = n;
                        Connector::synth_conn.input_port_name = Connector::in_name_list[n];
                        Connector::checkOpenPorts();
                    }
                    ImGui::MouseCursorToHand();
                    if (is_selected) ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::PopStyleVar();
            ImGui::EndCombo();
        }
        ImGui::MouseCursorToHand();
        ImGui::PopItemWidth();
        ImGui::SameLine();
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 4.0f));
        if (ImGui::ImageButton((void*)(intptr_t)Image::getTextureId(Image::Texture::Reload), ImVec2(14.0f, 14.0f)))
        {
            Connector::resetAllConnections();
        }
        ImGui::MouseCursorToHand();
        ImGui::PopStyleVar(3);
        ImGui::PopFont();

        drawOptionItem("Synth Output Device");
        ImGui::PushFont((int)Font::OptionItem);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, ITEM_PADDING_Y));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12.0f, ITEM_SPACING_Y));
        ImGui::PushItemWidth(220.0f);
        if (ImGui::BeginCombo("##SYNTH OUT", Connector::synth_conn.output_port_name.c_str()))
        {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12.0f, 8.0f));
            for (int n = 0; n < Connector::out_name_list.size(); ++n)
            {
                bool is_selected = n == Connector::synth_conn.output_port_index;
                if (ImGui::Selectable(Connector::out_name_list[n].c_str(), is_selected))
                {
                    Connector::synth_conn.output_port_index = n;
                    Connector::synth_conn.output_port_name = Connector::out_name_list[n];
                    Connector::checkOpenPorts();
                }
                ImGui::MouseCursorToHand();
                if (is_selected) ImGui::SetItemDefaultFocus();
            }
            ImGui::PopStyleVar();
            ImGui::EndCombo();
        }
        ImGui::MouseCursorToHand();
        ImGui::PopItemWidth();
        ImGui::PopStyleVar(2);
        ImGui::PopFont();

        drawOptionItem("Keyboard Input Device");
        ImGui::PushFont((int)Font::OptionItem);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, ITEM_PADDING_Y));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12.0f, ITEM_SPACING_Y));
        ImGui::PushItemWidth(220.0f);
        if (ImGui::BeginCombo("##KEY IN", Connector::key_conn.input_port_name.c_str()))
        {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12.0f, 8.0f));
            for (int n = 0; n < Connector::in_name_list.size(); ++n)
            {
                bool is_selected = n == Connector::key_conn.input_port_index;
                if (n == Connector::synth_conn.input_port_index)
                {
                    ImGui::TextColoredU32(UI_COLOR_TEXT_DEV_USED, "[USED] %s", Connector::in_name_list[n].c_str());
                }
                else
                {
                    if (ImGui::Selectable(Connector::in_name_list[n].c_str(), is_selected))
                    {
                        Connector::key_conn.input_port_index = n;
                        Connector::key_conn.input_port_name = Connector::in_name_list[n];
                        Connector::keyOpenPort();
                    }
                    ImGui::MouseCursorToHand();
                    if (is_selected) ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::PopStyleVar();
            ImGui::EndCombo();
        }
        ImGui::MouseCursorToHand();
        ImGui::PopItemWidth();
        ImGui::PopStyleVar(2);
        ImGui::PopFont();

        drawOptionItem("Force Adjust Kb MIDI Channel");
        ImGui::PushFont((int)Font::OptionItem);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12.0f, ITEM_SPACING_Y));
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, ITEM_PADDING_Y));
        ImGui::Checkbox("##FORCE MIDI CH", &Connector::force_adjust_midi_channel);
        ImGui::MouseCursorToHand();
        ImGui::PopStyleVar(2);
        ImGui::PopFont();

        drawOptionItem("SysEx Delay");
        ImGui::PushFont((int)Font::OptionItem);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12.0f, ITEM_SPACING_Y));
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, ITEM_PADDING_Y));
        ImGui::PushItemWidth(160.0f);
        ImGui::SliderInt("##SysEx Delay", &Connector::store_delay_duration, 200, 500);
        ImGui::PopItemWidth();
        ImGui::MouseCursorToHand();
        ImGui::PopStyleVar(2);
        ImGui::PopFont();
    }
    ImGui::EndGroupPanel();
}

// DSI: Streichfett
void drawOptionGroupDeviceSettingInfo()
{
    ImGui::BeginGroupPanel("Device Setting Info", ImVec2(320.0f, 200.0f));
    {
        bool displayable_synth_info = Connector::isSynthConnected();

        drawOptionItem("Synth Device");
        ImGui::PushStyleColor(ImGuiCol_Text, displayable_synth_info ? UI_COLOR_TEXT_CONNECTED : UI_COLOR_TEXT_DISCONNECTED);
        //drawOptionItemValue(displayable_synth_info ? "Connected" : "Disconnected");
        if (displayable_synth_info) drawOptionItemValue(TEXT_CONNECTED, true);
        else drawOptionItemValue(TEXT_DISCONNECTED, true);
        ImGui::PopStyleColor();

        drawOptionItem("Firmware Version");
        //drawOptionItemValue(displayable_synth_info && MessageHandler::inquiry_dump.received ? MessageHandler::inquiry_dump.firmware_version.c_str() : TEXT_INFO_NONE);
        drawOptionItemValue(MessageHandler::inquiry_dump.firmware_version.c_str(), displayable_synth_info && MessageHandler::inquiry_dump.received);

        GlobalModel::Global* global = InternalSetting::getGlobalData();
        drawOptionItem("Device ID");
        /*
        displayable_synth_info ? drawOptionItemValue(global->device_id)
            : drawOptionItemValue(TEXT_INFO_NONE);
            */
        drawOptionItemValue(global->device_id, displayable_synth_info);
        drawOptionItem("MIDI Channel");
        /*
        displayable_synth_info ? drawOptionItemValue(global->midi_channel)
            : drawOptionItemValue(TEXT_INFO_NONE);
            */
        drawOptionItemValue(global->midi_channel, displayable_synth_info);
        drawOptionItem("Tuning");
        /*
        displayable_synth_info ? drawOptionItemValue(global->tuning)
            : drawOptionItemValue(TEXT_INFO_NONE);
            */
        drawOptionItemValue(global->tuning, displayable_synth_info);
        drawOptionItem("Transpose");
        /*
        displayable_synth_info ? drawOptionItemValue(global->transpose)
            : drawOptionItemValue(TEXT_INFO_NONE);
            */
        drawOptionItemValue(global->transpose, displayable_synth_info);
        drawOptionItem("Pitch Bend Range");
        /*
        displayable_synth_info ? drawOptionItemValue(global->pitch_bend_range)
            : drawOptionItemValue(TEXT_INFO_NONE);
            */
        drawOptionItemValue(global->pitch_bend_range, displayable_synth_info);
    }
    ImGui::EndGroupPanel();
}

void drawOptionPanel()
{
    ImGui::PushFont((int)Font::Section);
    drawOptionGroupConnections();
    drawOptionGroupDeviceSettingInfo();
    ImGui::PopFont();
}

} // Gui
} // StreichfettSse