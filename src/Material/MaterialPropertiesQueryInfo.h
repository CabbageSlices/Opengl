#ifndef __MATERIALPROPERTIESQUERYINFO_H__
#define __MATERIALPROPERTIESQUERYINFO_H__

#include <memory>
#include <string>
#include <vector>

#include "./UniformBlockQueryInfo.h"
#include "GraphicsWrapper.h"
#include "ShaderProgram/ShaderProgram.h"

class GLTextureObject;

// block query but specifically for materials to support texture data

class MaterialPropertiesQueryInfo : public UniformBlockQueryInfo {
  public:
    MaterialPropertiesQueryInfo();
    virtual ~MaterialPropertiesQueryInfo() = default;
    void queryBlockData(std::shared_ptr<ShaderProgram> shaderToQueryFrom, std::string blockNameInShader) override;
    void clearAllData() override;
    bool isDataLoaded() override;

    // checks if the uniform blocks have the same layout
    // this means they have the same attributes and the attributes are in the same order so all hte offsets are the same
    // this also means they havev the same textures (though the texture unit for the textures could be different)
    // HOWEVER this does not mean the lbock index/binding indices are teh same
    virtual bool haveSameLayout(const MaterialPropertiesQueryInfo &ref);

    // checks if the binding indices of all items are the same between the two blocks.
    // in uniform blocks this is just the block binding index. Inherited members might have more binding elements
    // that need to be the same.
    // again ISNT the block index
    // checks that the textuers have hte same texture unit (the binding value in glsl)
    virtual bool haveSameBindings(const MaterialPropertiesQueryInfo &ref);

    std::vector<std::string> getTextureNames();

    // stores texture names, NOT suffixed by sampler
    std::map<std::string, GLint> textureUnitForSamplerByTextureName;

    // need to know if a texture is provided to glsl or not.
    // it is done by adding a flag to all material uniform blocks for a given texture to indicate if it's provided or not.
    // in the ujniform block the texture attribute name will be {{textureName}}{{materialTextureProvidedFlagSuffix}}
    // ex: if texture name is "diffuseTexture" and the suffix is "_Provided", then the attribute name should be
    // "diffuseTexture_Provided" within the material block.
    static const std::string materialTextureProvidedFlagSuffix;

    // suffix to add to texture sampler for a given texture name. so final name of sampler within shader should be
    //{{textureName}}{{textureSamplerSuffix}}
    static std::string textureSamplerSuffix;

  private:
    std::string extractTextureNameFromAttributeName(const std::string &uniformAttributeName);
    std::string textureAttributeNameToSamplerName(const std::string &uniformAttributeName);

    bool isTextureInfoLoaded;
};

#endif  // __MATERIALPROPERTIESQUERYINFO_H__