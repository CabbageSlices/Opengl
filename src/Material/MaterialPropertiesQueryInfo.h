// #include <memory>

// #include "GraphicsWrapper.h"
// #include "ShaderProgram/ShaderProgram.h"

// class GLTextureObject;

// class MaterialPropertiesQueryInfo {
//   public:
//     MaterialPropertiesQueryInfo(std::shared_ptr<ShaderProgram> shaderToQueryFrom, string materialNameInShader);

//   private:
//     void queryMaterialData();

//     GLsizei materialBlockSize;
//     GLuint materialUniformBlockIndex;  // NOT BINDING INDEX
//     GLint materialUniformBlockBindingIndexInShader;

//     std::map<std::string, GLint> attributeOffsets;
//     std::map<std::string, GLint> textureSamplerTextureUnit;

//     std::shared_ptr<ShaderProgram> shaderUsedToQueryInfo;
//     string materialBlockNameInShader;
// };

// /*
// build mateiral properties query from a material block name as follows:

// use http://docs.gl/gl4/glGetUniformBlockIndex to get the block index
// use http://docs.gl/gl4/glGetActiveUniformBlock to get the block binding point
// use ^ to get block size (for buffer size)
// use ^ to get the number o factive uniforms to initialize an array for the uniform indices
// use ^ to get the indices of each attribute within the block
// use http://docs.gl/gl4/glGetActiveUniformsiv to get the length of the name of each of the uniform attributes, that way we
// can initialize a buffer for each name use http://docs.gl/gl4/glGetActiveUniform for each attribtue with the given name
// buffer size to get the name, numer of elements (for arrays), and type of each attribute use
// http://docs.gl/gl4/glGetActiveUniformsiv for each attribute to get the uniform offset to each attribute

// after all attribtues and uniform block info is loaded, need to laod texture sampler units.
// every boolean attribute that is suffixed by _provided in the uniform block corresponds to the name of a texture.
// use http://docs.gl/gl4/glGetUniformLocation with the texture sampler name to get the location of the texture sampler
// use http://docs.gl/gl4/glGetUniform to get the value of the sampler, which is it's binding texture unit
// */