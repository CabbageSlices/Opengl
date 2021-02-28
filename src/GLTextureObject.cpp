#include "GLTextureObject.h"

GLTextureObject::GLTextureObject(TextureType _textureType) : textureType(_textureType), textureObject(0) {
    glCreateTextures(_textureType, 1, &textureObject);
}

bool GLTextureObject::create(unsigned numMipMapLevels, SizedColourFormats sizedColorFormat, unsigned width, unsigned height,
                             ColourFormats colourFormat, DataType dataType, void *dataAtLowestLevel) {
    if (height == 0 || width == 0) {
        cout << "Could not create gltextureobject; width or height are not valid";
        return false;
    }

    if (!dataAtLowestLevel) {
        cout << "supplied empty data, texture storage created but object has no data";
    }

    if (numMipMapLevels == 0) {
        cout << "Error creating texture object; num mip map levels cannot be 0";
        return false;
    }

    if (textureObject == 0) {
        glCreateTextures(textureType, 1, &textureObject);
    }

    glTextureStorage2D(textureObject, numMipMapLevels, sizedColorFormat, width, height);

    if (dataAtLowestLevel) {
        glTextureSubImage2D(textureObject, 0, 0, 0, width, height, colourFormat, dataType, dataAtLowestLevel);
    }

    properties.numMipMapLevels = numMipMapLevels;
    properties.sizedColorFormat = sizedColorFormat;
    properties.width = width;
    properties.height = height;
    properties.colourFormat = colourFormat;
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

void GLTextureObject::bindToTextureUnit(unsigned textureUnit) { bindTextureUnit(textureUnit, textureObject); }