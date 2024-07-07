#include "common.hpp"
#include "data/internal_patch.hpp"
#include "gui/gui.hpp"
#include "gui/gui_color.hpp"
#include "gui/gui_font.hpp"
#include "midi/connector.hpp"
#include "midi/message_task.hpp"
//#include "model/sound_value_util.hpp"     // TODO delete toDvFunc

namespace ImGui
{

// DSI: Streichfett
// TODO: std::format(C++20) -> try to replace pointer-val and ref-value
bool NiSliderInt(const char* label, StreichfettSse::Ev* v, const char* format)
{
    return SliderInt(label, v->valp, v->min(), v->max(), format, ImGuiSliderFlags_NoInput);
}

// DSI: Streichfett
// TODO: std::format(C++20) -> try to replace pointer-val and ref-value
bool NiDragInt(const char* label, StreichfettSse::Ev* v, float v_speed, const char* format)
{
    return DragInt(label, v->valp, v_speed, v->min(), v->max(), format, ImGuiSliderFlags_NoInput);
}

} // ImGui

namespace StreichfettSse
{
namespace Gui
{

// DSI: Streichfett
const float COMMON_LABEL_WIDTH = 55.0f;
const float STRINGS_LABEL_WIDTH = 90.0f;
const float SOLO_LABEL_WIDTH = 90.0f;
const float EFFECT_LABEL_WIDTH = 55.0f;

// DSI: Streichfett
const float COMMON_CONTROL_WIDTH = 457.0f;
const float STRINGS_CONTROL_WIDTH = 144.0f;
const float SOLO_CONTROL_WIDTH = 144.0f;
const float EFFECT_CONTROL_WIDTH = 179.0f;

const float NEXT_GROUP_SPACE = 4.0f;

void drawSameLine()
{
    ImGui::SameLine(0.0f, 2.0f);
}

void helpMarker(const char* desc)
{
    ImGui::Text("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

/* TODO delete toDvFunc
void setWheelControl(Ev* const v, const int param_index,
    int (*vfn)(const int) = nullptr)
    */
void setWheelControl(Ev& v, int param_index)
{
    ImGuiIO& io = ImGui::GetIO();
    if (ImGui::IsItemHovered() && io.MouseWheel != 0.0f)
    {
        if (io.MouseWheel > 0) ++v;
        if (io.MouseWheel < 0) --v;
        /* TODO delete toDvFunc
        int send_value = vfn == nullptr ? v->ev() : vfn(v->ev());
        MessageTask::addParamChangedTask(param_index, send_value);
        */
        MessageTask::addParamChangedTask(param_index, v.toDv());
        Annotation::clearText();
        InternalPatch::current_patch_changed = true;
    }
}

void drawParamValueContextMenu(const char* label,
    Ev& v, const Ev& original_v,
    //const int param_index, int (*vfn)(const int) = nullptr)   // TODO delete toDvFunc
    int param_index)
{
    if (ImGui::BeginPopupContextItem())
    {
        GuiUtil::PushFont((int)Font::TextBold);
        ImGui::Text(label);
        ImGui::PopFont();

        GuiUtil::PushFont((int)Font::Text);
        if (ImGui::Selectable("Revert to the original value"))
        {
            v = original_v;
            /* TODO delete toDvFunc
            int send_value = vfn == nullptr ? v->ev() : vfn(v->ev());
            MessageTask::addParamChangedTask(param_index, send_value);
            */
            MessageTask::addParamChangedTask(param_index, v.toDv());
            Annotation::clearText();
            InternalPatch::current_patch_changed = true;
            ImGui::CloseCurrentPopup();
        }
        GuiUtil::MouseCursorToHand();
        if (ImGui::Selectable("Reset to the initial value"))
        {
            v.setDefault();
            /* TODO delete toDvFunc
            int send_value = vfn == nullptr ? v->ev() : vfn(v->ev());
            MessageTask::addParamChangedTask(param_index, send_value);
            */
            MessageTask::addParamChangedTask(param_index, v.toDv());
            Annotation::clearText();
            InternalPatch::current_patch_changed = true;
            ImGui::CloseCurrentPopup();
        }
        GuiUtil::MouseCursorToHand();

        ImGui::PopFont();
        ImGui::EndPopup();
    }
}

void drawSlider(const char* label, int label_index, float label_width, float control_width,
    Ev& v, const Ev& original_v,
    //const int param_index = -1, int(*vfn)(const int) = nullptr,   // TODO delete toDvFunc
    int param_index = -1,
    bool hide_label = false)
{
    const char* display_format = v.hasArr() ? v.evs() : "%d";
    bool changed_from_org = v.ev() != original_v.ev();
    if (changed_from_org) ImGui::PushStyleColor(ImGuiCol_Border, UI_COLOR_PARAM_CHANGED);
    if (GuiUtil::ImGuiLeftLabel(ImGui::NiSliderInt, label_index, label, hide_label,
        label_width, control_width, changed_from_org, UI_COLOR_PARAM_CHANGED,
        &v, display_format))
    {
        /* TODO delete toDvFunc
        int send_value = vfn == nullptr ? v->ev() : vfn(v->ev());
        MessageTask::addParamChangedTask(param_index, send_value);
        */
        MessageTask::addParamChangedTask(param_index, v.toDv());
        Annotation::clearText();
        InternalPatch::current_patch_changed = true;
    }
    if (changed_from_org) ImGui::PopStyleColor();
    GuiUtil::MouseCursorToHand();
    //setWheelControl(v, param_index, vfn); // TODO delete toDvFunc
    setWheelControl(v, param_index);

    //drawParamValueContextMenu(label, v, original_v, param_index, vfn);    // TODO delete toDvFunc
    drawParamValueContextMenu(label, v, original_v, param_index);
}

void drawCombo(const char* label, int label_index, float label_width, float control_width,
    Ev& v, const Ev& original_v,
    int param_index = -1, bool hide_label = false)
{
    bool changed_from_org = v.ev() != original_v.ev();
    if (changed_from_org) ImGui::PushStyleColor(ImGuiCol_Border, UI_COLOR_PARAM_CHANGED);
    if (GuiUtil::ImGuiLeftLabel(ImGui::BeginCombo, label_index, label, hide_label,
        label_width, control_width, changed_from_org, UI_COLOR_PARAM_CHANGED,
        v.evs(), (int)ImGuiComboFlags_None))
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12.0f, 8.0f));
        for (int n = 0; n < v.evArraySize(); ++n)
        {
            bool is_selected = n == v.ev();
            ImGui::PushStyleColor(ImGuiCol_Text, UI_COLOR_TEXT_BASE);
            if (ImGui::Selectable(v.getEvArrayItem(n), is_selected))
            {
                v = n;
                int send_value = v.ev();
                MessageTask::addParamChangedTask(param_index, send_value);
                Annotation::clearText();
                InternalPatch::current_patch_changed = true;
            }
            GuiUtil::MouseCursorToHand();
            ImGui::PopStyleColor();
            if (is_selected) ImGui::SetItemDefaultFocus();
        }
        ImGui::PopStyleVar();
        ImGui::EndCombo();
    }
    if (changed_from_org) ImGui::PopStyleColor();
    GuiUtil::MouseCursorToHand();

    drawParamValueContextMenu(label, v, original_v, param_index);
}

// DSI: Streichfett
void drawPatchSelector()
{
    ImGui::Indent(10.0f);

    ImGui::PushStyleColor(ImGuiCol_Text, UI_COLOR_TEXT_PATCH_INFO);
    GuiUtil::PushFont((int)Font::PatchInfo);
    auto& sound_addr = InternalPatch::getCurrentSoundAddress();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    for (int patch_i = 0; patch_i < 12; ++patch_i)
    {
        char bb = InternalPatch::getSoundBankChar(patch_i);
        int bs = InternalPatch::getSoundPatchNumber(patch_i);
        char buf[4];
        sprintf(buf, "%c%d", bb, bs);
        ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
        if (ImGui::Selectable(buf, patch_i == sound_addr.sound, 0, ImVec2(40.0f, 20.0f)))
        {
            InternalPatch::setCurrentSoundAddress(patch_i);
            setNextState(State::RequestGlobal);
        }
        ImGui::PopStyleVar();

        ImVec2 p0 = ImGui::GetItemRectMin();
        ImVec2 p1 = ImGui::GetItemRectMax();

        ImU32 sel_bg;
        switch (patch_i / 4)
        {
            case 0:
                sel_bg = IM_COL32(120, 40, 80, 100);
                break;
            case 1:
                sel_bg = IM_COL32(80, 120, 40, 100);
                break;
            case 2:
                sel_bg = IM_COL32(40, 80, 120, 100);
                break;
            default:
                sel_bg = IM_COL32(0, 0, 0, 100);
                break;
        }
        draw_list->AddRectFilled(p0, p1, sel_bg);
        GuiUtil::MouseCursorToHand();

        if (patch_i == sound_addr.sound)
        {
            draw_list->AddRect(p0, p1, IM_COL32(255, 255, 255, 120));
        }

        if (patch_i != 11) ImGui::SameLine();
    }
    ImGui::PopFont();
    ImGui::PopStyleColor();

    ImGui::Unindent(10.0f);
}

// DSI: Streichfett
void drawPatchOperators(SoundModel::Patch& cp)
{
    bool current_patch_changed = InternalPatch::current_patch_changed;

    ImGui::Indent(4.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 2.0f));
    GuiUtil::PushFont((int)Font::Text);

    if (!current_patch_changed) ImGui::BeginDisabled();
    else
    {
        ImGui::PushStyleColor(ImGuiCol_Button, UI_COLOR_DMP_BTN);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, UI_COLOR_DMP_BTN_HOVERED);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, UI_COLOR_DMP_BTN_ACTIVE);
    }
    if (ImGui::Button("Send Sound Dump"))
    {
        reservedFuncs.push_back(std::bind(Connector::sendSoundDump, false));
    }
    GuiUtil::MouseCursorToHand();
    if (!current_patch_changed) ImGui::EndDisabled();
    else ImGui::PopStyleColor(3);

    ImGui::SameLine();

    if (ImGui::Button(">"))
    {
        ImGui::OpenPopup("patch_operators");
    }
    GuiUtil::MouseCursorToHand();
    if (ImGui::BeginPopup("patch_operators"))
    {
        GuiUtil::PushFont((int)Font::TextBold);
        ImGui::Text("Operation to Edit Buffer");
        ImGui::PopFont();
        if (ImGui::Selectable("Initialize All Parameters"))
        {
            cp.init();
            reservedFuncs.push_back(std::bind(Connector::sendSoundDump, true));
            Annotation::clearText();
            InternalPatch::current_patch_changed = true;
        }
        GuiUtil::MouseCursorToHand();
        ImGui::EndPopup();
    }

    ImGui::SameLine(0.0f, 365.0f);
    if (ImGui::Button("All Sound Off"))
    {
        reservedFuncs.push_back(std::bind(Connector::sendAllSoundOff));
        Annotation::clearText();
    }
    GuiUtil::MouseCursorToHand();

    ImGui::PopFont();
    ImGui::PopStyleVar();
    ImGui::Unindent(4.0f);
}

// DSI: Streichfett
void drawPatchParameters(SoundModel::Patch& cp, SoundModel::Patch& op)
{
    using namespace SoundModel;

    GuiUtil::PushFont((int)Font::TextBold);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 4.0f));

    GuiUtil::BeginGroupPanel("Common", ImVec2(100.0f, 100.0f));
    {
        auto label_width = COMMON_LABEL_WIDTH;
        auto control_width = COMMON_CONTROL_WIDTH;

        GuiUtil::PushFont((int)Font::Text);

        /* TODO delete toDvFunc
        drawSlider("Balance", 0, label_width, control_width,
            &cp->balance, &op->balance, PARAM_CC.at(ParamIndex::Balance),
            SoundValueUtil::centerEvToDv);
            */
        drawSlider("Balance", 0, label_width, control_width,
            cp.balance, op.balance, PARAM_CC.at(ParamIndex::Balance));

        ImGui::PopFont();
    }
    GuiUtil::EndGroupPanel();

    GuiUtil::BeginGroupPanel("Strings", ImVec2(100.0f, 100.0f));
    {
        auto label_width = STRINGS_LABEL_WIDTH;
        auto control_width = STRINGS_CONTROL_WIDTH;

        GuiUtil::PushFont((int)Font::Text);

        drawSlider("Registration", 0, label_width, control_width,
            cp.registration, op.registration, PARAM_CC.at(ParamIndex::Registration));

        drawSlider("Octaves", 0, label_width, control_width,
            cp.octave_switch, op.octave_switch, PARAM_CC.at(ParamIndex::OctaveSwitch));

        drawSlider("Ensemble", 0, label_width, control_width,
            cp.ensemble, op.ensemble, PARAM_CC.at(ParamIndex::Ensemble));

        drawCombo("Ensemble Type", 0, label_width, control_width,
            cp.ensemble_effect, op.ensemble_effect, PARAM_CC.at(ParamIndex::EnsembleEffect));

        drawSlider("Crescendo", 0, label_width, control_width,
            cp.crescendo, op.crescendo, PARAM_CC.at(ParamIndex::Crescendo));

        drawSlider("Release", 0, label_width, control_width,
            cp.release, op.release, PARAM_CC.at(ParamIndex::Release));

        ImGui::PopFont();
    }
    GuiUtil::EndGroupPanel();

    ImGui::SameLine();

    GuiUtil::BeginGroupPanel("Solo", ImVec2(100.0f, 100.0f));
    {
        auto label_width = SOLO_LABEL_WIDTH;
        auto control_width = SOLO_CONTROL_WIDTH;

        GuiUtil::PushFont((int)Font::Text);

        drawSlider("Tone", 0, label_width, control_width,
            cp.tone, op.tone, PARAM_CC.at(ParamIndex::Tone));

        drawSlider("Tremolo", 0, label_width, control_width,
            cp.tremolo, op.tremolo, PARAM_CC.at(ParamIndex::Tremolo));

        drawCombo("Split", 0, label_width, control_width,
            cp.split_layer, op.split_layer, PARAM_CC.at(ParamIndex::SplitLayer));

        drawSlider("Spliy key", 0, label_width, control_width,
            //&cp->split_key, &op->split_key, -1, nullptr, false);  // TODO delete toDvFunc
            cp.split_key, op.split_key, -1, false);
        drawSameLine();
        helpMarker("Changes to this parameter will take effect the next time you load.");

        drawCombo("Envelope Mode", 0, label_width, control_width,
            cp.envelope_mode, op.envelope_mode, PARAM_CC.at(ParamIndex::EnvelopeMode));

        drawSlider("Attack", 0, label_width, control_width,
            cp.attack, op.attack, PARAM_CC.at(ParamIndex::Attack));

        drawSlider("Decay(Release)", 0, label_width, control_width,
            cp.decay, op.decay, PARAM_CC.at(ParamIndex::Decay));

        ImGui::PopFont();
    }
    GuiUtil::EndGroupPanel();

    GuiUtil::BeginGroupPanel("Effect", ImVec2(100.0f, 100.0f));
    {
        auto label_width = EFFECT_LABEL_WIDTH;
        auto control_width = EFFECT_CONTROL_WIDTH;

        GuiUtil::PushFont((int)Font::Text);

        drawSlider("Animate", 0, label_width, control_width,
            cp.animate, op.animate, PARAM_CC.at(ParamIndex::Animate));

        drawSlider("Phaser", 0, label_width, control_width,
            cp.phaser, op.phaser, PARAM_CC.at(ParamIndex::Phaser));

        drawSlider("Reverb", 0, label_width, control_width,
            cp.reverb, op.reverb, PARAM_CC.at(ParamIndex::Reverb));

        ImGui::PopFont();
    }
    GuiUtil::EndGroupPanel();

    ImGui::PopStyleVar();
    ImGui::PopFont();
}

void drawEditPanel(SoundModel::Patch& cp, SoundModel::Patch& op)
{
    bool is_synth_connected = Connector::isSynthConnected();

    if (!is_synth_connected) ImGui::BeginDisabled();
    ImGui::Dummy(ImVec2(10.0f, 10.0f));
    drawPatchSelector();
    ImGui::Dummy(ImVec2(10.0f, 4.0f));
    drawPatchOperators(cp);
    ImGui::Dummy(ImVec2(10.0f, 10.0f));
    drawPatchParameters(cp, op);
    if (!is_synth_connected) ImGui::EndDisabled();
}

} // Gui
} // StreichfettSse