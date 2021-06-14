#include "TextureProperties.h"

const std::map<TextureParameterGroup, std::vector<TextureParameterType>> textureParametersInParameterGroup{
    {TextureParameterGroup::ZOOM, {TextureParameterType::MIN_FILTER, TextureParameterType::MAG_FILTER}},
    {TextureParameterGroup::TEXTURE_WRAP,
     {TextureParameterType::TEXTURE_WRAP_T, TextureParameterType::TEXTURE_WRAP_S, TextureParameterType::TEXTURE_WRAP_R}}};