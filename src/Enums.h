#pragma once
#include "glad/glad.h"

/**
 * @brief Enum mapping a sized format to opengls sized internal formats
 *
 */
enum SizedColourFormats {
    RGB_8 = GL_RGB8,
    RGBA_8 = GL_RGBA8,
};

enum ColourFormats {
    RGB = GL_RGB,
    RGBA = GL_RGBA,
};

enum DataType { FLOAT = GL_FLOAT, UNSIGNED_BYTE = GL_UNSIGNED_BYTE };

enum TextureType { TEXTURE_2D = GL_TEXTURE_2D };