#include "Material.h"

unsigned Material::NUM_MATERIALS = 1;

void Material::connectMaterialDataToShader() {
    materialBuffer.bindToTargetBindingPoint(MATERIAL_UNIFORM_BLOCK_BINDING_POINT);

    for (auto &texture : textures) {
        texture.second->bindToTextureUnit(texture.first);
    }
}

void Material::setSpecularCoefficient(float specular) {
    diffuseMaterial.specularCoefficient = specular;
    updateBufferData();
}

void Material::setDiffuseColour(glm::vec4 _diffuse) {
    diffuseMaterial.diffuse = _diffuse;
    updateBufferData();
}
void Material::setTexture(TextureUnit textureUnit, std::unique_ptr<GLTextureObject> &textureObject) {
    diffuseMaterial.isDiffuseTextureAvailable = 1;
    textures.insert({textureUnit, std::move(textureObject)});
    updateBufferData();
}