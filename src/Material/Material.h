#pragma once
#include <any>
#include <map>
#include <string>
#include <typeinfo>

#include "Buffer.h"
#include "GLTextureObject.h"
#include "Includes.h"
#include "ShaderProgram/shaderprogram.h"
#include "error_strings.h"
#include "glm/glm.hpp"
#include "samplerBindings.frag"

struct Attribute {
    shared_ptr<void> data;
    const std::type_info *type;
    size_t size;
};

class Material {
  public:
    Material(std::shared_ptr<ShaderProgram> &program, string materialNameInShader, bool prefixAttributeNameWithMaterialName);
    Material(string materialNameInShader, bool prefixAttributeNameWithMaterialName);

    template <typename T>
    void setAttribute(string name, const T &value) {
        // if this attribute didn't exist then we need to get the offset for it, otherwise we don't care
        if (attributes.count(name) == 0) {
            loadedAttributeOffsets = false;
        }

        attributes[name] = Attribute{.data = shared_ptr<T>(new T(value)), .type = &typeid(value), .size = sizeof(T)};

        bufferRequiresUpdate = true;
    }

    // overload for bool, opengl stores bool as ints so we must store bool as ints
    void setAttribute(string name, const bool &value) {
        // if this attribute didn't exist then we need to get the offset for it, otherwise we don't care
        if (attributes.count(name) == 0) {
            loadedAttributeOffsets = false;
        }

        // store bool typeid to differenciate between bool and int in the attributes list.
        // that way when fetching a boolean we can check types with bool and can't retreive the data as an int
        attributes[name] =
            Attribute{.data = shared_ptr<int>(new int(value ? 1 : 0)), .type = &typeid(value), .size = sizeof(int)};

        bufferRequiresUpdate = true;
    }

    template <typename T>
    T getAttribute(const string &name) {
        if (attributes.count(name) == 0) {
            throw NOT_FOUND_ERROR;
        }

        auto &attribute = attributes[name];

        if (*attribute.type != typeid(T)) {
            throw INCORRECT_TYPE_ERROR;
        }

        return *(T *)(attribute.data.get());
    }

    bool getAttribute(const string &name) {
        if (attributes.count(name) == 0) {
            throw NOT_FOUND_ERROR;
        }

        auto &attribute = attributes[name];

        if (*attribute.type != typeid(bool)) {
            throw INCORRECT_TYPE_ERROR;
        }

        int value = *(int *)(attribute.data.get());

        return value == 1;
    }

    // texture name should not contain the _included or _sampler.
    void setTexture(string name, shared_ptr<GLTextureObject> texture);

    bool activate();

    void setShader(std::shared_ptr<ShaderProgram> &program) {
        // no program initially so all the buffers need to be setup for the first time
        // even if it's a fixed uniform buffer in GLSL, we haven't set up the initial offsets etc so it must be done atleast
        // once with a real shader
        if (!shaderProgram && program) {
            loadedAttributeOffsets = false;
            loadedUniformBlockData = false;
            bufferRequiresUpdate = true;

            // textures only require updates if there are textures loaded
            textureUnitsRequireUpdate = textures.size() > 0;
        }
        shaderProgram = program;
    }

    int getId() { return id; }

    GLuint getBufferObject() { return buffer.getBufferObject(); }

    static std::string materialTextureProvidedFlagPrefix;
    static std::string samplerTextureSamplerPrefix;

    std::shared_ptr<ShaderProgram> getProgram() { return shaderProgram; }

    static std::shared_ptr<ShaderProgram> createShaderProgramForDefaultMaterial();
    static std::shared_ptr<Material> createDefaultMaterial();

  private:
    // load information about the uniform block in the sahder such as the binding location, the index
    bool queryUniformBlockInformationInShader();

    // load the offsets to each of the attributes
    bool queryAttributeOffsets();
    bool updateBuffer();
    bool updateTextureUnits();

    void activateTextures();

    bool loadedAttributeOffsets;
    bool loadedUniformBlockData;
    bool bufferRequiresUpdate;
    bool textureUnitsRequireUpdate;

    bool prefixAttributeWithMaterialName;

    GLsizei uniformBlockSize;

    // map attribute name to value
    std::map<std::string, Attribute> attributes;

    // map attribute name to offset to the attribute within the uniform block in the shader
    std::map<std::string, GLint> attributeOffsets;

    std::map<std::string, std::shared_ptr<GLTextureObject>> textures;
    std::map<std::string, GLint> textureSamplerTextureUnit;

    std::shared_ptr<ShaderProgram> shaderProgram;
    string materialName;
    GLuint materialUniformBlockIndex;  // unique id of the uniform block within the shader. This isn't the binding unit, but
    // is needed to query data for this block
    GLint bindingIndexInShader;

    Buffer buffer;

    int id;

    static int numMaterialsCreated;
};