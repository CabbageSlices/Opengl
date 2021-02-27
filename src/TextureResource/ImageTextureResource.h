#pragma once

#include "Enums.h"
#include "Includes.h"

class GLTextureObject;

/**
 * @brief Deleter for stbi objects
 *
 * @param image pointer to stbi image that needs to be cleaned up
 */
inline void deleteStbiImage(stbi_uc *image) {
    if (image) stbi_image_free(image);
}

/**
 * @brief Object used to load a texture from disk into memory. Does not handle
 * opengl texture objects, simply loads the texture, and keeps track of the
 * texture's dimensions etc.
 *
 */
class ImageTextureResource {
  public:
    enum class Channels : int { GREY = 1, GREYALPHA = 2, RGB = 3, RGBA = 4, AUTO = 0 };
    /**
     * @brief Construct a new Texture Resource object by loading the image from
     * the given file.
     *
     * @param fileName name of file to load from, MUST include directory path, etc
     * @param desiredChannels how many channels to store when loaded. Image can
     * have any number of channels, and this will force the loaded data to store
     * the given number of channels.
     * @throw throws exception if texture failed to load from file for whatever
     * reason
     */
    ImageTextureResource(const std::string &fileName, const Channels &desiredChannels = Channels::AUTO);

    ImageTextureResource &operator=(const ImageTextureResource &rhs) = delete;
    ImageTextureResource(const ImageTextureResource &rhs) = delete;

    /**
     * @brief Create a GLTextureObject from this resource. Sets the parameters
     * correctly according to the texture attributes. Returns null pointer on
     * failure. requires a texture loaded Also gives the option to automatically generate the various mipmap levels (user can
     * do it on his own as well).
     *
     * @param numAdditionalMipMapLevels the number of additional mipmap levels to add to the texture object. This does not
     * include the base level, which is mipmap level.
     * @param autoGenerateMipMapLevels when set to true all higher mipmap levels are generated automatically
     * @return std::shared_ptr<GLTextureObject>
     */
    std::shared_ptr<GLTextureObject> createGLTextureObject(unsigned int numAdditionalMipMapLevels,
                                                           bool autoGenerateMipMapLevels = false);

    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getChannelnsInFile() const { return channelsInFile; }

    /**
     * @brief Get the number of loaded channels. if the texture was loaded with desired channels set to auto then  this value
     * will be equal to the number of channels in the texture. Otherwise this value will be the same as the value used to
     * load the texture.
     *
     * @return Channels
     */
    Channels getLoadedChannels() const { return loadedChannels; }

    SizedColourFormats getSizedColourFormat() const;
    ColourFormats getColourFormat() const;

    const stbi_uc *getData() const { return data.get(); }

  private:
    static const std::map<Channels, SizedColourFormats> ChannelsToSizedColourFormatMap;
    static const std::map<Channels, ColourFormats> ChannelsToColourFormatMap;

    std::unique_ptr<stbi_uc, decltype(&deleteStbiImage)> data;

    int width;
    int height;

    int channelsInFile;
    Channels loadedChannels;
};