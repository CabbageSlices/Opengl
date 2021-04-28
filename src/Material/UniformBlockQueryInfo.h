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

    // checks if the uniform blocks have the same layout
    // this means they have the same attributes and the attributes are in the same order so all hte offsets are the same
    // HOWEVER this does not mean the lbock index/binding indices are teh same
    virtual bool haveSameLayout(const UniformBlockQueryInfo &ref);

    // checks if the binding indices of all items are the same between the two blocks.
    // in uniform blocks this is just the block binding index. Inherited members might have more binding elements
    // that need to be the same.
    // again ISNT the block index
    virtual bool haveSameBindings(const UniformBlockQueryInfo &ref);

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