#include "./Material.h"

std::string Material::materialTextureProvidedFlagPrefix = "_Provided";
std::string Material::samplerTextureSamplerPrefix = "_Sampler";
int Material::numMaterialsCreated = 0;

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

std::shared_ptr<ShaderProgram> Material::createShaderProgramForDefaultMaterial() {
    std::shared_ptr<ShaderProgram> defaultShaderProgram = std::make_shared<ShaderProgram>();
    defaultShaderProgram->loadAndCompileShaders(
        {{Shader::Type::Vertex, "vertex.vert"}, {Shader::Type::Fragment, "fragment.frag"}});
    defaultShaderProgram->linkProgram();

    return defaultShaderProgram;
}

std::shared_ptr<Material> Material::createDefaultMaterial() {
    auto shaderProgram = createShaderProgramForDefaultMaterial();

    auto material = std::make_shared<Material>(shaderProgram, "DiffuseMaterial", false);
    material->setAttribute("diffuseColor", glm::vec4(1, 1, 1, 1));
    material->setAttribute("specularCoefficient", 225);
    material->setAttribute("diffuseTexture_Provided", false);

    return material;
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