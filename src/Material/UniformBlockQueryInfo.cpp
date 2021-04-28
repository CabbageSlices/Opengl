#include "UniformBlockQueryInfo.h"

string UniformBlockQueryInfo::removeBlockPrefixFromAttributeName(const string &attributeName) {
    size_t positionOfPeriod = attributeName.find(".");

    // attribute names prefixed by block name, need to remove it
    if (positionOfPeriod == string::npos) {
        return attributeName;
    }

    const size_t lengthOfBlockNamePrefix = positionOfPeriod + 1;

    return attributeName.substr(lengthOfBlockNamePrefix);
}

UniformBlockQueryInfo::UniformBlockQueryInfo()
    : uniformBlockSize(0),
      uniformBlockIndex(0),
      uniformBlockBindingIndexInShader(0),
      attributeOffsets(),
      blockNamePrefixRequiredForAttributeNames(false),
      isQueryInfoLoaded(false),
      shaderUsedToQueryInfo(),
      uniformBlockNameInShader() {}

void UniformBlockQueryInfo::queryBlockData(std::shared_ptr<ShaderProgram> shaderToQueryFrom, string blockNameInShader) {
    clearAllData();
    shaderUsedToQueryInfo = shaderToQueryFrom;
    uniformBlockNameInShader = blockNameInShader;

    // get uniform block index
    uniformBlockIndex = shaderUsedToQueryInfo->getUniformBlockIndex(uniformBlockNameInShader);

    if (uniformBlockIndex == GL_INVALID_INDEX) {
        throw "Uniform block " + uniformBlockNameInShader + " does not exist in the specified shader, failed to query info";
    }

    // get material size data
    uniformBlockSize =
        shaderUsedToQueryInfo->getUniformBlockProperty(uniformBlockIndex, UniformBlockProperty::UniformBlockSizeInBytes);

    // material block binding index
    uniformBlockBindingIndexInShader =
        shaderUsedToQueryInfo->getUniformBlockProperty(uniformBlockIndex, UniformBlockProperty::UniformBlockBindingIndex);

    // get names of all attributes
    shaderUsedToQueryInfo->getUniformBlockActiveUniformIndices(uniformBlockIndex);

    // get names of all attributes
    vector<GLuint> activeUniformIndices = shaderUsedToQueryInfo->getUniformBlockActiveUniformIndices(uniformBlockIndex);
    map<GLint, string> uniformIndexToAttributeName;
    map<GLint, GLenum> uniformIndexToType;
    map<GLint, GLint> uniformIndexToNumberOfElementsIfArray;

    shaderUsedToQueryInfo->queryUniformAttributeDataFromIndex(activeUniformIndices, uniformIndexToAttributeName,
                                                              uniformIndexToType, uniformIndexToNumberOfElementsIfArray);

    map<GLuint, GLint> offsets = shaderUsedToQueryInfo->getUniformOffsetForAttributes(activeUniformIndices);

    for (auto &pair : offsets) {
        GLint attributeIndex = pair.first;
        GLint offset = pair.second;
        string name = uniformIndexToAttributeName[attributeIndex];
        blockNamePrefixRequiredForAttributeNames = name.find(uniformBlockNameInShader) != string::npos;

        name = removeBlockPrefixFromAttributeName(name);

        attributeOffsets[name] = offset;
    }

    isQueryInfoLoaded = true;
}

void UniformBlockQueryInfo::clearAllData() {
    uniformBlockSize = 0;
    uniformBlockIndex = 0;
    uniformBlockBindingIndexInShader = -1;
    isQueryInfoLoaded = false;

    attributeOffsets.clear();

    blockNamePrefixRequiredForAttributeNames = false;
}

bool UniformBlockQueryInfo::haveSameLayout(const UniformBlockQueryInfo &ref) {
    if (!isQueryInfoLoaded || !ref.isQueryInfoLoaded) {
        return false;
    }

    if (uniformBlockSize != ref.uniformBlockSize) {
        return false;
    }

    if (attributeOffsets.size() != ref.attributeOffsets.size()) {
        return false;
    }

    for (const auto &pair : attributeOffsets) {
        if (ref.attributeOffsets.count(pair.first) == 0) {
            return false;
        }

        if (ref.attributeOffsets.at(pair.first) != pair.second) {
            return false;
        }
    }

    return true;
}

bool UniformBlockQueryInfo::haveSameBindings(const UniformBlockQueryInfo &ref) {
    if (!isQueryInfoLoaded || !ref.isQueryInfoLoaded) {
        return false;
    }
    return uniformBlockBindingIndexInShader == ref.uniformBlockBindingIndexInShader;
}

bool UniformBlockQueryInfo::isDataLoaded() { return isQueryInfoLoaded; }