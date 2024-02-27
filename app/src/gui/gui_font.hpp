#pragma once

namespace StreichfettSse
{
namespace Gui
{

enum class Font : int
{
    Title,
    Version,
    OptionItem,
    OptionItemBold,
    Section,
    PatchInfo,
    PatchName,
    Text,
    TextBold,
    _COUNT_,
};

#ifdef _DEBUG
enum class FontDebug : int
{
    Text = static_cast<int>(Font::_COUNT_),
    ProcHead,
    ProcHex,
    _COUNT_,
};
#endif

void addAllFonts();
#ifdef _DEBUG
void addAllFontsDebug();
#endif

} // Gui
} // StreichfettSse
