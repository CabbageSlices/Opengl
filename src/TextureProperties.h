#pragma once
#include <any>
#include <map>

#include "GLTextureObject.h"
#include "GraphicsWrapper.h"

enum class SizedColourFormats {
    RGB_8 = GL_RGB8,
    RGBA_8 = GL_RGBA8,
};

enum class SizedDepthFormats {
    DEPTH_COMPONENT32F = GL_DEPTH_COMPONENT32F,
    DEPTH_COMPONENT = GL_DEPTH_COMPONENT,
    DEPTH_COMPONENT16 = GL_DEPTH_COMPONENT16,
    DEPTH_COMPONENT24 = GL_DEPTH_COMPONENT24
};

enum class TextureInternalStorageFormat {
    DEPTH_COMPONENT32F = GL_DEPTH_COMPONENT32F,
    DEPTH_COMPONENT32 = GL_DEPTH_COMPONENT32,
    DEPTH_COMPONENT = GL_DEPTH_COMPONENT,
    DEPTH_COMPONENT16 = GL_DEPTH_COMPONENT16,
    DEPTH_COMPONENT24 = GL_DEPTH_COMPONENT24,
    RGB_8 = GL_RGB8,
    RGBA_8 = GL_RGBA8,
};

enum class PixelDataFormat : GLenum {
    RGB = GL_RGB,
    RGBA = GL_RGBA,
    DEPTH_COMPONENT = GL_DEPTH_COMPONENT,
};

enum class TextureParameterType : GLenum {
    MIN_FILTER = GL_TEXTURE_MIN_FILTER,
    MAG_FILTER = GL_TEXTURE_MAG_FILTER,
    TEXTURE_WRAP_S = GL_TEXTURE_WRAP_S,
    TEXTURE_WRAP_T = GL_TEXTURE_WRAP_T
};

enum class TextureFilteringModes : GLint {
    LINEAR = GL_LINEAR,
};

enum class TextureWrapModes : GLint { CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE };

struct TextureProperties {
    unsigned numMipMapLevels;
    TextureInternalStorageFormat internalFormat;
    unsigned width;
    unsigned height;
    PixelDataFormat formatOfData;
    DataType dataType;

    std::map<TextureParameterType, std::any> parameters;
};