#pragma once

#include "GraphicsWrapper.h"

/**
 * @brief Enum mapping a sized format to opengls sized internal formats
 *
 */
enum SizedColourFormats {
    RGB_8 = GL_RGB8,
    RGBA_8 = GL_RGBA8,
    DEPTH_COMPONENT_32F = GL_DEPTH_COMPONENT32F,
};

enum ColourFormats {
    RGB = GL_RGB,
    RGBA = GL_RGBA,
};

enum DataType { FLOAT = GL_FLOAT, UNSIGNED_BYTE = GL_UNSIGNED_BYTE };

enum TextureType { TEXTURE_2D = GL_TEXTURE_2D };