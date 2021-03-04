#pragma once
#include "Enums.h"
#include "Includes.h"
#include "TextureProperties.h"

/**
 * @brief Store texture data inside an opengl texture object. Allows people to manipulate opengl textur objects.
 *
 */
class GLTextureObject {
  public:
    GLTextureObject(TextureType _textureType);
    virtual ~GLTextureObject() { deleteTexture(); }
    GLTextureObject(const GLTextureObject &rhs) = delete;
    GLTextureObject &operator=(const GLTextureObject &rhs) = delete;

    /**
     * @brief Delete the stored texture object if it exists.
     *
     */
    void deleteTexture() {
        if (textureObject == 0) return;

        glDeleteTextures(1, &textureObject);
        textureObject = 0;
    }

    GLuint getTextureObject() const { return textureObject; }
    /**
     * @brief Create the storage for the texture, and fill the lowest level of the mipmap layers (level 0) with the specified
     * data The given data must be in the formats specified, with the given dimensions
     * DOES NOT CHECK FOR OPENGL ERRORS
     *
     * @param numMipMapLevels
     * @param sizedColorFormat format of the data internally in the open gl texture object
     * @param width
     * @param height
     * @param dataFormat format of the given pointer of data
     * @param data
     * @return true when texture data successfully created
     * @return false there is an error with the data values.
     */
    bool create(unsigned numMipMapLevels, TextureInternalStorageFormat internalFormat, unsigned width, unsigned height,
                PixelDataFormat colourFormat, DataType, void *dataAtLowestLevel);

    /**
     * @brief Automatically generate the mipmap levels for everything but the base level (level 0)
     *
     * @return true if there is a texture object exists
     * @return false if texture object doesn't exist
     */
    bool generateMipMapLevels();

    /**
     * @brief Binds the texture object to the given texture unit, if texture is 0 then it will clear the bound texture
     *
     * @param textureUnit  texture unit to bind this texture to
     */
    void bindToTextureUnit(unsigned textureUnit);

    void setParameter(const TextureParameterType &parameterType, const TextureFilteringModes &parameterValue);
    void setParameter(const TextureParameterType &parameterType, const TextureWrapModes &parameterValue);

    /**
     * @brief Clears whatever texture is currently bound to the given texture unit. Essentially sets the bound texture to 0
     *
     * @param textureUnit
     */
    static void unbindTextureAtUnit(unsigned textureUnit) { glBindTextureUnit(textureUnit, 0); }

  private:
    TextureProperties properties;
    TextureType textureType;
    GLuint textureObject;
};