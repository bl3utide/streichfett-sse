#include "common.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "image.hpp"
#include "compressed/arrayed_texture.hpp"

namespace StreichfettSse
{
namespace Image
{

// private
GLuint _texture[static_cast<int>(Texture::_COUNT_)];

bool loadTextureFromMemory(const unsigned char buffer[], int size, GLuint* output)
{
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load_from_memory(buffer, size, &image_width, &image_height, NULL, 4);
    if (image_data == nullptr)
        return false;

    GLuint image_texture = 0;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);

    *output = image_texture;

    return true;
}

void unloadTexture(const GLuint* texture) noexcept
{
    if (texture != nullptr)
        glDeleteTextures(1, texture);
}

void loadTextures()
{
    bool load_tex_ret;

    load_tex_ret = loadTextureFromMemory(
        ArrayedTexture::TEX_ICON,
        ArrayedTexture::TEX_ICON_SIZE,
        &_texture[static_cast<int>(Texture::Icon)]);
    IM_ASSERT(load_tex_ret);

    load_tex_ret = loadTextureFromMemory(
        ArrayedTexture::TEX_RELOAD,
        ArrayedTexture::TEX_RELOAD_SIZE,
        &_texture[static_cast<int>(Texture::Reload)]);
    IM_ASSERT(load_tex_ret);
}

void unloadTextures()
{
    unloadTexture(&_texture[static_cast<int>(Texture::Reload)]);
}

void initialize()
{
    loadTextures();
}

void finalize() noexcept
{
    unloadTextures();
}

GLuint getTextureId(Texture tex) noexcept
{
    return _texture[static_cast<int>(tex)];
}

} // Image
} // StreichfettSse
