#pragma once

#include "GraphicsWrapper.h"

enum DataType { FLOAT = GL_FLOAT, UNSIGNED_BYTE = GL_UNSIGNED_BYTE };

enum TextureType {
    TEXTURE_2D = GL_TEXTURE_2D,
    TEXTURE_CUBEMAP = GL_TEXTURE_CUBE_MAP,
    TEXTURE_2D_ARRAY = GL_TEXTURE_2D_ARRAY,
    TEXTURE_CUBEMAP_ARRAY = GL_TEXTURE_CUBE_MAP_ARRAY
};

enum FramebufferColorAttachment : GLenum {
    NONE = GL_NONE,
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