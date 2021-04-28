#pragma once
#include <any>
#include <map>
#include <string>
#include <typeinfo>

#include "Buffer.h"
#include "GLTextureObject.h"
#include "Includes.h"
#include "Material/MaterialPropertiesQueryInfo.h"
#include "ShaderProgram/shaderprogram.h"
#include "error_strings.h"
#include "glm/glm.hpp"
#include "samplerBindings.frag"

using std::map;
using std::shared_ptr;
using std::string;

struct Attribute {
    shared_ptr<void> data;
    const std::type_info *type;
    size_t size;
};

class Material {
  public:
    static std::shared_ptr<ShaderProgram> getShaderProgramForDefaultMaterial();
    static std::shared_ptr<Material> createDefaultMaterial();

    Material();
    Material(const shared_ptr<MaterialPropertiesQueryInfo> &_queryInfo);

    Material(const Material &m1);
    Material &operator=(const Material &rhs);

    template <typename T>
    void setAttribute(string name, const T &value) {
        attributes[name] = Attribute{.data = shared_ptr<T>(new T(value)), .type = &typeid(value), .size = sizeof(T)};

        bufferRequiresUpdate = true;
    }

    // overload for bool, opengl stores bool as ints so we must store bool as ints
    void setAttribute(string name, const bool &value) {
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

    int getId() { return id; }

    GLuint getBufferObject() { return buffer.getBufferObject(); }

    void setQueryInfo(const shared_ptr<MaterialPropertiesQueryInfo> &_queryInfo, const bool &skipBufferUpdateCheck) {
        if (!skipBufferUpdateCheck && queryInfo && _queryInfo && !queryInfo->haveSameLayout(*_queryInfo)) {
            bufferRequiresUpdate = true;
        }
        queryInfo = _queryInfo;
    }

    std::shared_ptr<MaterialPropertiesQueryInfo> getQueryInfo() { return queryInfo; }

    bool isBufferUpdateRequired() const { return bufferRequiresUpdate; }
    void requireBufferUpdate() { bufferRequiresUpdate = true; }

  private:
    // load information about the uniform block in the sahder such as the binding location, the index
    bool queryUniformBlockInformationInShader();

    bool updateBuffer();
    void activateTextures();

    shared_ptr<MaterialPropertiesQueryInfo> queryInfo;

    // map attribute name to value
    std::map<std::string, Attribute> attributes;

    std::map<std::string, std::shared_ptr<GLTextureObject>> textures;

    bool bufferRequiresUpdate;
    Buffer buffer;

    int id;

    static int numMaterialsCreated;

    static shared_ptr<ShaderProgram> programForDefaultMaterial;
};

void copyAttributes(const map<string, Attribute> &source, map<string, Attribute> &destination);
void copyTextures(const map<string, shared_ptr<GLTextureObject>> &src, map<string, shared_ptr<GLTextureObject>> &dst);