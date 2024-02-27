#include "common.hpp"
#include "gui/gui_font.hpp"

namespace StreichfettSse
{
namespace Gui
{

struct Font
{
    void* data;
    int data_size;
    float pixels;

    Font(void* const font_data, int font_data_size, float font_pixels)
        : data(font_data), data_size(font_data_size), pixels(font_pixels)
    { }
};

} // Gui
} // StreichfettSse
