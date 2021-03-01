#include "Enums.h"
#include "GraphicsWrapper.h"

struct TextureProperties {
    unsigned numMipMapLevels;
    SizedColourFormats sizedColorFormat;
    unsigned width;
    unsigned height;
    ColourFormats colourFormat;
    DataType dataType;
};