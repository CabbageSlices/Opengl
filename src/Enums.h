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

enum FramebufferColorAttachment : GLenum {
    COLOR_ATTACHMENT0 = GL_COLOR_ATTACHMENT0,
    COLOR_ATTACHMENT1 = GL_COLOR_ATTACHMENT1,
    COLOR_ATTACHMENT2 = GL_COLOR_ATTACHMENT2,
    COLOR_ATTACHMENT3 = GL_COLOR_ATTACHMENT3,
};

enum FramebufferNonColorAttachment : GLenum {
    DEPTH_ATTACHMENT = GL_DEPTH_ATTACHMENT,
    STENCIL_ATTACHMENT = GL_STENCIL_ATTACHMENT,
    DEPTH_STENCIL_ATTACHMENT = GL_DEPTH_STENCIL_ATTACHMENT,
};

enum FramebufferTarget : GLenum {
    FRAMEBUFFER = GL_FRAMEBUFFER,
};