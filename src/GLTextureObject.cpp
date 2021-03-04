#include "GLTextureObject.h"

GLTextureObject::GLTextureObject(TextureType _textureType) : textureType(_textureType), textureObject(0) {
    glCreateTextures(_textureType, 1, &textureObject);
}

bool GLTextureObject::create(unsigned numMipMapLevels, TextureInternalStorageFormat internalFormat, unsigned width,
                             unsigned height, PixelDataFormat formatOfData, DataType dataType, void *dataAtLowestLevel) {
    if (height == 0 || width == 0) {
        cout << "Could not create gltextureobject; width or height are not valid" << endl;
        return false;
    }

    if (!dataAtLowestLevel) {
        cout << "supplied empty data, texture storage created but object has no data" << endl;
    }

    if (numMipMapLevels == 0) {
        cout << "Error creating texture object; num mip map levels cannot be 0" << endl;
        return false;
    }

    if (textureObject == 0) {
        glCreateTextures(textureType, 1, &textureObject);
    }

    glTextureStorage2D(textureObject, numMipMapLevels, (GLenum)internalFormat, width, height);

    if (dataAtLowestLevel) {
        glTextureSubImage2D(textureObject, 0, 0, 0, width, height, (GLenum)formatOfData, dataType, dataAtLowestLevel);
    }

    properties.numMipMapLevels = numMipMapLevels;
    properties.internalFormat = internalFormat;
    properties.width = width;
    properties.height = height;
    properties.formatOfData = formatOfData;
    properties.dataType = dataType;

    return true;
}

bool GLTextureObject::generateMipMapLevels() {
    if (textureObject == 0) {
        cout << "Error geenrating texture mipmaps; no texture object detected" << endl;
        return false;
    }

    glGenerateTextureMipmap(textureObject);

    return true;
}

void GLTextureObject::setParameter(const TextureParameterType &parameterType, const TextureFilteringModes &parameterValue) {
    if (textureObject == 0) {
        throw "No texture created";
    }

    glTextureParameteri(textureObject, (GLenum)parameterType, (GLint)parameterValue);

    properties.parameters[parameterType] = parameterValue;
}
void GLTextureObject::setParameter(const TextureParameterType &parameterType, const TextureWrapModes &parameterValue) {
    if (textureObject == 0) {
        throw "No texture created";
    }

    glTextureParameteri(textureObject, (GLenum)parameterType, (GLint)parameterValue);

    properties.parameters[parameterType] = parameterValue;
}

void GLTextureObject::bindToTextureUnit(unsigned textureUnit) { bindTextureUnit(textureUnit, textureObject); }