#include "TextureResource/ImageTextureResource.h"

#include <stdexcept>

#include "GLTextureObject.h"

typedef ImageTextureResource::Channels Channels;

const std::map<Channels, SizedColourFormats> ImageTextureResource::ChannelsToSizedColourFormatMap = {
    {Channels::RGB, SizedColourFormats::RGB_8}, {Channels::RGBA, SizedColourFormats::RGBA_8}};

const std::map<Channels, ColourFormats> ImageTextureResource::ChannelsToColourFormatMap = {
    {Channels::RGB, ColourFormats::RGB}, {Channels::RGBA, ColourFormats::RGBA}};

ImageTextureResource::ImageTextureResource(const std::string &fileName,
                                           const ImageTextureResource::Channels &desiredChannels)
    : data(nullptr, &deleteStbiImage),
      width(0),
      height(0),
      channelsInFile(0),
      loadedChannels(desiredChannels) {
    data = std::unique_ptr<stbi_uc, decltype(&deleteStbiImage)>(
        stbi_load(fileName.c_str(), &width, &height, &channelsInFile, static_cast<int>(loadedChannels)), &deleteStbiImage);

    if (!data) {
        throw "Failed to load texture form file " + fileName;
    }

    // auto loaded channels has the same channels as the file itself, need to save the number of loaded channels
    if (loadedChannels == Channels::AUTO) {
        loadedChannels = static_cast<Channels>(channelsInFile);
    }
}

std::shared_ptr<GLTextureObject> ImageTextureResource::createGLTextureObject(unsigned int numAdditionalMipMapLevels,
                                                                             bool autoGenerateMipMapLevels) {
    if (!data) {
        return nullptr;
    }

    std::shared_ptr<GLTextureObject> textureObject(new GLTextureObject(TextureType::TEXTURE_2D));

    try {
        textureObject->create(1 + numAdditionalMipMapLevels, getSizedColourFormat(), width, height, getColourFormat(),
                              DataType::UNSIGNED_BYTE, data.get());
    } catch (...) {
        cout << "Error creating texture resource" << endl;
        return nullptr;
    }

    if (autoGenerateMipMapLevels) {
        textureObject->generateMipMapLevels();
    }

    return textureObject;
}

SizedColourFormats ImageTextureResource::getSizedColourFormat() const {
    try {
        return ChannelsToSizedColourFormatMap.at(loadedChannels);
    } catch (const std::out_of_range &error) {
        cout << "Sized colour format not defined for this image channels" << endl;
        throw;
    }
}

ColourFormats ImageTextureResource::getColourFormat() const {
    try {
        return ChannelsToColourFormatMap.at(loadedChannels);
    } catch (const std::out_of_range &error) {
        cout << "colour format not defined for this image channels" << endl;
        throw;
    }
}