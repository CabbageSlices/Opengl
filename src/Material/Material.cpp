#include "./Material.h"

std::string Material::materialTextureProvidedFlagPrefix = "_Provided";
std::string Material::samplerTextureSamplerPrefix = "_Sampler";
int Material::numMaterialsCreated = 0;
shared_ptr<ShaderProgram> Material::programForDefaultMaterial = nullptr;

std::shared_ptr<ShaderProgram> Material::getShaderProgramForDefaultMaterial() {
    if (!programForDefaultMaterial) {
        programForDefaultMaterial = std::make_shared<ShaderProgram>();
        programForDefaultMaterial->loadAndCompileShaders(
            {{Shader::Type::Vertex, "vertex.vert"}, {Shader::Type::Fragment, "fragment.frag"}});
        programForDefaultMaterial->linkProgram();
    }

    return programForDefaultMaterial;
}

std::shared_ptr<Material> Material::createDefaultMaterial() {
    auto shaderProgram = getShaderProgramForDefaultMaterial();

    auto material = std::make_shared<Material>(shaderProgram, "DiffuseMaterial", false);
    material->setAttribute("diffuseColor", glm::vec4(1, 1, 1, 1));
    material->setAttribute("specularCoefficient", 225);
    material->setAttribute("diffuseTexture_Provided", false);

    return material;
}

void copyAttributes(const std::map<std::string, Attribute> &source, std::map<std::string, Attribute> &destination) {
    for (auto &attributePair : source) {
        string name = attributePair.first;
        Attribute attribute = attributePair.second;

        void *copiedData = malloc(attribute.size);
        cout << " DATA MADE: " << copiedData << endl;
        memcpy(copiedData, attribute.data.get(), attribute.size);

        shared_ptr<void> dataPtr(copiedData, [](void *data) {
            cout << "DATA FREED: " << data << endl;
            free(data);
        });

        destination[name] = Attribute{.data = dataPtr, .type = attribute.type, .size = attribute.size};
    }
}

void copyTextures(const map<string, shared_ptr<GLTextureObject>> &src, map<string, shared_ptr<GLTextureObject>> &dst) {
    for (auto &texturePair : src) {
        string name = texturePair.first;
        auto textureObj = texturePair.second;

        dst[name] = textureObj;
    }
}

Material::Material(std::shared_ptr<ShaderProgram> &program, string materialNameInShader,
                   bool prefixAttributeNameWithMaterialName)
    : loadedAttributeOffsets(false),
      loadedUniformBlockData(false),
      bufferRequiresUpdate(true),
      textureUnitsRequireUpdate(false),
      prefixAttributeWithMaterialName(prefixAttributeNameWithMaterialName),
      uniformBlockSize(0),
      shaderProgram(program),
      materialName(materialNameInShader),
      materialUniformBlockIndex(GL_INVALID_INDEX),
      bindingIndexInShader(GL_INVALID_INDEX),
      id(++numMaterialsCreated) {
    buffer.create(Buffer::UniformBuffer, NULL, 0, Buffer::StaticDraw);
}

Material::Material(string materialNameInShader, bool prefixAttributeNameWithMaterialName)
    : loadedAttributeOffsets(false),
      loadedUniformBlockData(false),
      bufferRequiresUpdate(true),
      textureUnitsRequireUpdate(false),
      prefixAttributeWithMaterialName(prefixAttributeNameWithMaterialName),
      uniformBlockSize(0),
      shaderProgram(),
      materialName(materialNameInShader),
      materialUniformBlockIndex(GL_INVALID_INDEX),
      bindingIndexInShader(GL_INVALID_INDEX),
      id(++numMaterialsCreated) {
    buffer.create(Buffer::UniformBuffer, NULL, 0, Buffer::StaticDraw);
}

Material::Material(const Material &m1)
    : loadedAttributeOffsets(m1.loadedAttributeOffsets),
      loadedUniformBlockData(m1.loadedUniformBlockData),
      bufferRequiresUpdate(m1.bufferRequiresUpdate),
      textureUnitsRequireUpdate(m1.textureUnitsRequireUpdate),
      prefixAttributeWithMaterialName(m1.prefixAttributeWithMaterialName),
      uniformBlockSize(m1.uniformBlockSize),
      attributeOffsets(m1.attributeOffsets),
      textureSamplerTextureUnit(m1.textureSamplerTextureUnit),
      shaderProgram(m1.shaderProgram),
      materialName(m1.materialName),
      materialUniformBlockIndex(m1.materialUniformBlockIndex),
      bindingIndexInShader(m1.bindingIndexInShader),
      id(++numMaterialsCreated) {
    copyAttributes(m1.attributes, attributes);
    copyTextures(m1.textures, textures);
    updateBuffer();
}

Material &Material::operator=(const Material &rhs) {
    loadedAttributeOffsets = rhs.loadedAttributeOffsets;
    loadedUniformBlockData = rhs.loadedUniformBlockData;
    bufferRequiresUpdate = rhs.bufferRequiresUpdate;
    textureUnitsRequireUpdate = rhs.textureUnitsRequireUpdate;
    prefixAttributeWithMaterialName = rhs.prefixAttributeWithMaterialName;
    uniformBlockSize = rhs.uniformBlockSize;
    attributeOffsets = rhs.attributeOffsets;
    textureSamplerTextureUnit = rhs.textureSamplerTextureUnit;
    shaderProgram = rhs.shaderProgram;
    materialName = rhs.materialName;
    materialUniformBlockIndex = rhs.materialUniformBlockIndex;
    bindingIndexInShader = rhs.bindingIndexInShader;

    copyAttributes(rhs.attributes, attributes);
    copyTextures(rhs.textures, textures);
    updateBuffer();

    return *this;
}

bool Material::queryAttributeOffsets() {
    shared_ptr<ShaderProgram> program = shaderProgram;
    if (!program) {
        return false;
    }

    attributeOffsets.clear();

    vector<string> names;

    // name of attribute is going to be UniformBlock.attributeName, where uniformblock is the name of the uniformblock in
    // the shader
    for (auto &attribute : attributes) {
        const string name = prefixAttributeWithMaterialName ? materialName + "." + attribute.first : attribute.first;
        names.push_back(name);
    }

    attributeOffsets = program->getUniformOffsetForAttributes(names);

    // failed to load some offsets
    if (attributeOffsets.size() < attributes.size()) {
        cout << "ERROR: loading attribute offets for material: " << materialName << endl;
        return false;
    }

    loadedAttributeOffsets = true;
    bufferRequiresUpdate = true;

    return true;
}

bool Material::queryUniformBlockInformationInShader() {
    shared_ptr<ShaderProgram> program = shaderProgram;
    if (!program) {
        materialUniformBlockIndex = GL_INVALID_INDEX;
        bindingIndexInShader = GL_INVALID_INDEX;
        return false;
    }

    materialUniformBlockIndex = program->getUniformBlockIndex(materialName);

    if (materialUniformBlockIndex == GL_INVALID_INDEX) {
        cout << "ERROR: Material uniform block does not exist: " << materialName << endl;
        materialUniformBlockIndex = GL_INVALID_INDEX;
        bindingIndexInShader = GL_INVALID_INDEX;
        return false;
    }

    // get the binding index
    bindingIndexInShader =
        program->getUniformBlockProperty(materialUniformBlockIndex, UniformBlockProperty::UniformBlockBindingIndex);
    if (bindingIndexInShader == GL_INVALID_INDEX) {
        cout << "ERROR: unable to get material uniform block binding index: " << materialName << endl;
        return false;
    }

    uniformBlockSize =
        program->getUniformBlockProperty(materialUniformBlockIndex, UniformBlockProperty::UniformBlockSizeInBytes);

    loadedUniformBlockData = true;
    return true;
}

bool Material::updateBuffer() {
    if (!loadedAttributeOffsets && !queryAttributeOffsets()) {
        return false;
    }

    unsigned char *data = new unsigned char[uniformBlockSize]();
    for (auto &entry : attributes) {
        GLsizei offset = attributeOffsets[entry.first];
        memcpy(data + offset, entry.second.data.get(), entry.second.size);
    }

    buffer.create(Buffer::UniformBuffer, data, uniformBlockSize, Buffer::UsageType::StaticDraw);

    bufferRequiresUpdate = false;

    delete[] data;
    return true;
}

void Material::setTexture(string name, shared_ptr<GLTextureObject> texture) {
    // store the texture
    textures[name] = texture;
    setAttribute<bool>(name + materialTextureProvidedFlagPrefix, true);

    loadedAttributeOffsets = false;
    bufferRequiresUpdate = true;
    textureUnitsRequireUpdate = true;
}

bool Material::updateTextureUnits() {
    if (!shaderProgram) {
        return false;
    }

    textureSamplerTextureUnit.clear();

    for (auto &texture : textures) {
        string name = texture.first;
        GLint unit = shaderProgram->getUniform<GLint>(name + samplerTextureSamplerPrefix);

        if (unit == -1) {
            cout << "ERROR: unable to get texture sampler unit for texture: " << name << " ; in material: " << materialName
                 << endl;
            textureUnitsRequireUpdate = true;
            return false;
        }

        textureSamplerTextureUnit[name] = unit;
    }

    textureUnitsRequireUpdate = false;
    return true;
}

void Material::activateTextures() {
    for (auto &texture : textures) {
        string name = texture.first;
        auto textureObj = texture.second;

        GLint unit = textureSamplerTextureUnit[name];
        textureObj->bindToTextureUnit(unit);
    }
}

bool Material::activate() {
    shared_ptr<ShaderProgram> program = shaderProgram;

    if (!loadedUniformBlockData && !queryUniformBlockInformationInShader()) {
        return false;
    }

    if (!loadedAttributeOffsets && !queryAttributeOffsets()) {
        return false;
    }

    if (bufferRequiresUpdate && !updateBuffer()) {
        return false;
    }

    if (textureUnitsRequireUpdate && !updateTextureUnits()) {
        return false;
    }

    program->useProgram();
    buffer.bindToTargetBindingPoint(bindingIndexInShader);
    activateTextures();

    return true;
}