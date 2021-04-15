#ifndef __MATERIALPROPERTIESQUERYINFO_H__
#define __MATERIALPROPERTIESQUERYINFO_H__

#include <memory>

#include "./UniformBlockQueryInfo.h"
#include "GraphicsWrapper.h"
#include "ShaderProgram/ShaderProgram.h"

class GLTextureObject;

// block query but specifically for materials to support texture data

class MaterialPropertiesQueryInfo : public UniformBlockQueryInfo {
  public:
    MaterialPropertiesQueryInfo();
    virtual ~MaterialPropertiesQueryInfo() = default;
    void queryBlockData(std::shared_ptr<ShaderProgram> shaderToQueryFrom, string blockNameInShader) override;
    void clearAllData() override;
    bool isDataLoaded() override;

    vector<string> getTextureNames();

    // stores texture names, NOT suffixed by sampler
    std::map<std::string, GLint> textureUnitForSamplerByTextureName;

    // need to know if a texture is provided to glsl or not.
    // it is done by adding a flag to all material uniform blocks for a given texture to indicate if it's provided or not.
    // in the ujniform block the texture attribute name will be {{textureName}}{{materialTextureProvidedFlagSuffix}}
    // ex: if texture name is "diffuseTexture" and the suffix is "_Provided", then the attribute name should be
    // "diffuseTexture_Provided" within the material block.
    static const string materialTextureProvidedFlagSuffix;

    // suffix to add to texture sampler for a given texture name. so final name of sampler within shader should be
    //{{textureName}}{{textureSamplerSuffix}}
    static std::string textureSamplerSuffix;

  private:
    string extractTextureNameFromAttributeName(const string &uniformAttributeName);
    string textureAttributeNameToSamplerName(const string &uniformAttributeName);

    bool isTextureInfoLoaded;
};

#endif  // __MATERIALPROPERTIESQUERYINFO_H__