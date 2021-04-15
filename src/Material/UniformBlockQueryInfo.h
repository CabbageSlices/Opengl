#ifndef __UNIFORMBLOCKQUERYINFO_H__
#define __UNIFORMBLOCKQUERYINFO_H__

#include <memory>

#include "GraphicsWrapper.h"
#include "ShaderProgram/ShaderProgram.h"

class UniformBlockQueryInfo {
  public:
    static string removeBlockPrefixFromAttributeName(const string &attributeName);

    UniformBlockQueryInfo();
    virtual ~UniformBlockQueryInfo() = default;
    virtual void queryBlockData(std::shared_ptr<ShaderProgram> shaderToQueryFrom, string blockNameInShader);

    virtual bool isDataLoaded();

    virtual void clearAllData();

    std::shared_ptr<ShaderProgram> getShader() { return shaderUsedToQueryInfo; }

    string getUniformBlockName() { return uniformBlockNameInShader; }

    GLsizei uniformBlockSize;
    GLuint uniformBlockIndex;  // NOT BINDING INDEX
    GLint uniformBlockBindingIndexInShader;

    std::map<std::string, GLint> attributeOffsets;

    bool blockNamePrefixRequiredForAttributeNames;  // if lbock name prefix is required, then when accessing an attribute
                                                    // wihtin GLSL you NEEED the block name
    // BUUUT the attributeOffsets will NOT contain the block name, that way it's easier for users to use

  protected:
    bool isQueryInfoLoaded;
    std::shared_ptr<ShaderProgram> shaderUsedToQueryInfo;
    string uniformBlockNameInShader;
};

#endif  // __UNIFORMBLOCKQUERYINFO_H__