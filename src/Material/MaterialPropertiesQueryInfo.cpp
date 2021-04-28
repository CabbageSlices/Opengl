#include "MaterialPropertiesQueryInfo.h"

using std::string;
using std::vector;

const string MaterialPropertiesQueryInfo::materialTextureProvidedFlagSuffix = "_Provided";
std::string MaterialPropertiesQueryInfo::textureSamplerSuffix = "_Sampler";

MaterialPropertiesQueryInfo::MaterialPropertiesQueryInfo() : UniformBlockQueryInfo(), isTextureInfoLoaded(false) {}

void MaterialPropertiesQueryInfo::queryBlockData(std::shared_ptr<ShaderProgram> shaderToQueryFrom,
                                                 string blockNameInShader) {
    UniformBlockQueryInfo::queryBlockData(shaderToQueryFrom, blockNameInShader);

    vector<string> textureNames = getTextureNames();

    for (string &name : textureNames) {
        textureUnitForSamplerByTextureName[name] = shaderToQueryFrom->getUniform<GLint>(name + textureSamplerSuffix);
    }

    isTextureInfoLoaded = true;
}

void MaterialPropertiesQueryInfo::clearAllData() {
    UniformBlockQueryInfo::clearAllData();
    textureUnitForSamplerByTextureName.clear();
}

bool MaterialPropertiesQueryInfo::isDataLoaded() { return UniformBlockQueryInfo::isDataLoaded() && isTextureInfoLoaded; }

bool MaterialPropertiesQueryInfo::haveSameLayout(const MaterialPropertiesQueryInfo &ref) {
    if (!UniformBlockQueryInfo::haveSameLayout(ref)) {
        return false;
    }

    if (!isTextureInfoLoaded || !ref.isTextureInfoLoaded) {
        return false;
    }

    if (textureUnitForSamplerByTextureName.size() != ref.textureUnitForSamplerByTextureName.size()) {
        return false;
    }

    // have the same textgures but not necessiraily in the asme units
    for (const auto &pair : textureUnitForSamplerByTextureName) {
        if (ref.textureUnitForSamplerByTextureName.count(pair.first) == 0) {
            return false;
        }
    }

    return true;
}

bool MaterialPropertiesQueryInfo::haveSameBindings(const MaterialPropertiesQueryInfo &ref) {
    if (!isTextureInfoLoaded || !ref.isTextureInfoLoaded) {
        return false;
    }

    if (textureUnitForSamplerByTextureName.size() != ref.textureUnitForSamplerByTextureName.size()) {
        return false;
    }

    for (const auto &pair : textureUnitForSamplerByTextureName) {
        if (ref.textureUnitForSamplerByTextureName.count(pair.first) == 0) {
            return false;
        }

        if (ref.textureUnitForSamplerByTextureName.at(pair.first) != pair.second) {
            return false;
        }
    }

    return true;
}

vector<string> MaterialPropertiesQueryInfo::getTextureNames() {
    vector<string> textureNames;

    // if any of the attributes have the texture flag suffix then we know it's a texture name,
    for (auto &offsets : attributeOffsets) {
        const string attributeName = offsets.first;

        // not a texture, ignore
        if (attributeName.find(materialTextureProvidedFlagSuffix) == string::npos) {
            continue;
        }

        const string textureName = extractTextureNameFromAttributeName(attributeName);
        textureNames.push_back(textureName);
    }

    return textureNames;
}

string MaterialPropertiesQueryInfo::extractTextureNameFromAttributeName(const string &uniformAttributeName) {
    string textureName =
        uniformAttributeName.substr(0, uniformAttributeName.length() - materialTextureProvidedFlagSuffix.length());

    return UniformBlockQueryInfo::removeBlockPrefixFromAttributeName(textureName);
}