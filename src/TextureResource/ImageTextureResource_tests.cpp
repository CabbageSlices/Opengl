#include "./ImageTextureResource.h"
#include "Includes.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

// Test that loading an image to an imagetextureresource results in the data loading correctly
// no exception is thrown, and the image properties are loaded correctly
TEST(ImageTextureResource, loadFromImageSuccess) {
    const int width = 3;
    const int height = 3;
    const int channelsInFile = 3;
    const ImageTextureResource::Channels desiredChannels = ImageTextureResource::Channels::AUTO;

    try {
        ImageTextureResource image("testing_resources/images/3x3.jpg", ImageTextureResource::Channels::AUTO);

        ASSERT_EQ(image.getWidth(), width);
        ASSERT_EQ(image.getHeight(), height);
        ASSERT_EQ(image.getChannelnsInFile(), channelsInFile);
        ASSERT_EQ(image.getLoadedChannels(), desiredChannels);

        const stbi_uc *data = image.getData();

        ASSERT_NE(data, nullptr);

    } catch (std::string errorMessage) {
        FAIL() << "Image failed to load";
    }
}

// Test that loading a non existing image fails
TEST(ImageTextureResource, loadFromImageFailure) {
    try {
        ImageTextureResource image("testing_resources/images/3x3.tiff", ImageTextureResource::Channels::AUTO);

        ASSERT_EQ(image.getData(), nullptr);
        FAIL() << "LOADED NON EXISTING IMAGE OR NO EXCEDPTION THROWN";

    } catch (std::string errorMessage) {
        SUCCEED() << "failed to load image and exception thrown (THIS IS WHAT YOU WANT)";
    }
}