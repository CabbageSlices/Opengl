#include "./Material.h"

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
    shared_ptr<MaterialPropertiesQueryInfo> info(new MaterialPropertiesQueryInfo());
    info->queryBlockData(shaderProgram, "DiffuseMaterial");

    auto material = std::make_shared<Material>(info);
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
        memcpy(copiedData, attribute.data.get(), attribute.size);

        shared_ptr<void> dataPtr(copiedData, [](void *data) { free(data); });

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

Material::Material(const shared_ptr<MaterialPropertiesQueryInfo> &_queryInfo)
    : queryInfo(_queryInfo),
      bufferRequiresUpdate(true),
      id(++numMaterialsCreated) {
    buffer.create(Buffer::UniformBuffer, NULL, 0, Buffer::StaticDraw);
}

Material::Material() : bufferRequiresUpdate(true), id(++numMaterialsCreated) {
    buffer.create(Buffer::UniformBuffer, NULL, 0, Buffer::StaticDraw);
}

Material::Material(const Material &m1)
    : queryInfo(m1.queryInfo),
      bufferRequiresUpdate(m1.bufferRequiresUpdate),
      id(++numMaterialsCreated) {
    copyAttributes(m1.attributes, attributes);
    copyTextures(m1.textures, textures);
    updateBuffer();
}

Material &Material::operator=(const Material &rhs) {
    queryInfo = rhs.queryInfo;
    id = rhs.id;

    copyAttributes(rhs.attributes, attributes);
    copyTextures(rhs.textures, textures);
    updateBuffer();

    return *this;
}

bool Material::updateBuffer() {
    if (!queryInfo || !queryInfo->isDataLoaded()) {
        return false;
    }

    const GLsizei blockSize = queryInfo->uniformBlockSize;

    unsigned char *data = new unsigned char[blockSize]();
    for (auto &entry : attributes) {
        GLsizei offset = queryInfo->attributeOffsets.at(entry.first);
        memcpy(data + offset, entry.second.data.get(), entry.second.size);
    }

    buffer.create(Buffer::UniformBuffer, data, queryInfo->uniformBlockSize, Buffer::UsageType::StaticDraw);

    bufferRequiresUpdate = false;

    delete[] data;
    return true;
}

// TODO don't recreate entire buffer when updating texture, only update the flag
void Material::setTexture(string name, shared_ptr<GLTextureObject> texture) {
    // store the texture
    textures[name] = texture;
    setAttribute<bool>(name + MaterialPropertiesQueryInfo::materialTextureProvidedFlagSuffix, true);

    bufferRequiresUpdate = true;
}

void Material::activateTextures() {
    for (auto &texture : textures) {
        string name = texture.first;
        auto textureObj = texture.second;

        GLint unit = queryInfo->textureUnitForSamplerByTextureName.at(name);
        textureObj->bindToTextureUnit(unit);
    }
}

bool Material::activate() {
    if (!queryInfo || !queryInfo->isDataLoaded()) {
        return false;
    }

    if (bufferRequiresUpdate && !updateBuffer()) {
        return false;
    }

    shared_ptr<ShaderProgram> program = queryInfo->getShader();

    program->useProgram();
    buffer.bindToTargetBindingPoint(queryInfo->uniformBlockBindingIndexInShader);
    activateTextures();

    return true;
}