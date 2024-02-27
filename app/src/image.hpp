#pragma once

namespace StreichfettSse
{
namespace Image
{

enum class Texture : int
{
    Icon,
    Reload,
    _COUNT_,
};

void initialize();
void finalize() noexcept;
GLuint getTextureId(Texture tex) noexcept;

} // Image
} // StreichfettSse
