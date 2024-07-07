#include "common.hpp"
#include "compressed/arrayed_font.hpp"
#include "gui/gui_font.hpp"

namespace StreichfettSse
{
namespace Gui
{

struct TextFont
{
    const void* data;
    int data_size;
    float pixels;

    explicit TextFont(const void* font_data, int font_data_size, float font_pixels)
        : data(font_data), data_size(font_data_size), pixels(font_pixels)
    {}

    void add() const
    {
        auto font = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(
            data, data_size, pixels
        );
        IM_ASSERT(font);
    }
};

const std::unordered_map<Font, TextFont> FONTS
{
    { Font::Title,          TextFont(ArrayedFont::TITLE_DATA, ArrayedFont::TITLE_SIZE, 24.0f) },
    { Font::Version,        TextFont(ArrayedFont::SECTION_DATA, ArrayedFont::SECTION_SIZE, 16.0f) },
    { Font::OptionItem,     TextFont(ArrayedFont::SECTION_DATA, ArrayedFont::SECTION_SIZE, 18.0f) },
    { Font::OptionItemBold, TextFont(ArrayedFont::SECTION_BOLD_DATA, ArrayedFont::SECTION_BOLD_SIZE, 18.0f) },
    { Font::Section,        TextFont(ArrayedFont::SECTION_DATA, ArrayedFont::SECTION_SIZE, 22.0f) },
    { Font::PatchInfo,      TextFont(ArrayedFont::PATCH_NAME_DATA, ArrayedFont::PATCH_NAME_SIZE, 22.0f) },
    { Font::PatchName,      TextFont(ArrayedFont::PATCH_NAME_DATA, ArrayedFont::PATCH_NAME_SIZE, 36.0f) },
    { Font::Text,           TextFont(ArrayedFont::TEXT_DATA, ArrayedFont::TEXT_SIZE, 16.0f) },
    { Font::TextBold,       TextFont(ArrayedFont::TEXT_BOLD_DATA, ArrayedFont::TEXT_BOLD_SIZE, 16.0f) },
};

#ifdef _DEBUG
const std::unordered_map<FontDebug, TextFont> FONTS_DEBUG
{
    { FontDebug::Text,      TextFont(ArrayedFont::DEBUG_DATA, ArrayedFont::DEBUG_SIZE, 14.0f) },
    { FontDebug::ProcHead,  TextFont(ArrayedFont::DEBUG_DATA, ArrayedFont::DEBUG_SIZE, 20.0f) },
    { FontDebug::ProcHex,   TextFont(ArrayedFont::DEBUG_DATA, ArrayedFont::DEBUG_SIZE, 18.0f) },
};
#endif

void addAllFonts()
{
    const auto num_of_fonts = static_cast<int>(Font::_COUNT_);
    for (auto f_i = 0; f_i < num_of_fonts; ++f_i)
    {
        const auto f = static_cast<Font>(f_i);
        FONTS.at(f).add();
    }
}

#ifdef _DEBUG
void addAllFontsDebug()
{
    const auto num_of_fonts = static_cast<int>(FontDebug::_COUNT_);
    for (auto f_i = static_cast<int>(Font::_COUNT_); f_i < num_of_fonts; ++f_i)
    {
        const auto f = static_cast<FontDebug>(f_i);
        FONTS_DEBUG.at(f).add();
    }
}
#endif

} // Gui
} // StreichfettSse
