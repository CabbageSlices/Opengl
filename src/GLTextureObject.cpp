#include "GLTextureObject.h"

const std::map<TextureType, TextureDimensions> TEXTURE_DIMENSIONALITY{{
                                                                          TextureType::TEXTURE_2D,
                                                                          TextureDimensions::TWO,
                                                                      },
                                                                      {
                                                                          TextureType::TEXTURE_2D_ARRAY,
                                                                          TextureDimensions::THREE,
                                                                      },
                                                                      {
                                                                          TextureType::TEXTURE_CUBEMAP,
                                                                          TextureDimensions::THREE,
                                                                      }};

GLTextureObject::GLTextureObject(TextureType _textureType) : textureType(_textureType), textureObject(0) {
    glCreateTextures(_textureType, 1, &textureObject);
}

bool GLTextureObject::create(unsigned numMipMapLevels, TextureInternalStorageFormat internalFormat, GLsizei width,
                             GLsizei height) {
    if (height == 0 || width == 0) {
        cout << "Could not create gltextureobject; width or height are not valid" << endl;
        return false;
    }

    if (numMipMapLevels == 0) {
        cout << "Error creating texture object; num mip map levels cannot be 0" << endl;
        return false;
    }

    if (textureObject == 0) {
        glCreateTextures(textureType, 1, &textureObject);
    }

    glTextureStorage2D(textureObject, numMipMapLevels, (GLenum)internalFormat, width, height);

    properties.numMipMapLevels = numMipMapLevels;
    properties.internalFormat = internalFormat;
    properties.width = width;
    properties.height = height;

    return true;
}

bool GLTextureObject::create(unsigned numMipMapLevels, TextureInternalStorageFormat internalFormat, GLsizei width,
                             GLsizei height, GLsizei depth) {
    if (height == 0 || width == 0) {
        cout << "Could not create gltextureobject; width or height are not valid" << endl;
        return false;
    }

    if (numMipMapLevels == 0) {
        cout << "Error creating texture object; num mip map levels cannot be 0" << endl;
        return false;
    }

    if (textureObject == 0) {
        glCreateTextures(textureType, 1, &textureObject);
    }

    glTextureStorage3D(textureObject, numMipMapLevels, (GLenum)internalFormat, width, height, depth);

    properties.numMipMapLevels = numMipMapLevels;
    properties.internalFormat = internalFormat;
    properties.width = width;
    properties.height = height;
    properties.depth = depth;

    return true;
}

bool GLTextureObject::setTextureDataAtMipMapLevel(GLint mipmapLevel, PixelDataFormat formatOfData, DataType dataType,
                                                  void *data) {
    if (textureObject == 0) {
        cout << "No texture created, can't set data" << endl;
        return false;
    }

    if (TEXTURE_DIMENSIONALITY.at(textureType) == TextureDimensions::TWO) {
        glTextureSubImage2D(textureObject, mipmapLevel, 0, 0, properties.width, properties.height, (GLenum)formatOfData,
                            dataType, data);
    }
    if (TEXTURE_DIMENSIONALITY.at(textureType) == TextureDimensions::THREE) {
        glTextureSubImage3D(textureObject, mipmapLevel, 0, 0, 0, properties.width, properties.height, properties.depth,
                            (GLenum)formatOfData, dataType, data);
    }

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

void GLTextureObject::setParameterGroup(const TextureParameterGroup &parameterGroup,
                                        const TextureFilteringModes &parameterValue) {
    if (textureObject == 0) {
        throw "No texture created";
    }

    auto &textureParameterTypes = textureParametersInParameterGroup.at(parameterGroup);

    for (const auto &type : textureParameterTypes) {
        setParameter(type, parameterValue);
    }
}

void GLTextureObject::setParameterGroup(const TextureParameterGroup &parameterGroup,
                                        const TextureWrapModes &parameterValue) {
    if (textureObject == 0) {
        throw "No texture created";
    }

    auto &textureParameterTypes = textureParametersInParameterGroup.at(parameterGroup);

    for (const auto &type : textureParameterTypes) {
        setParameter(type, parameterValue);
    }
}

void GLTextureObject::bindToTextureUnit(unsigned textureUnit) { bindTextureUnit(textureUnit, textureObject); }