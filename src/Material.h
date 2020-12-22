#pragma once
#include <map>
#include <string>

#include "Buffer.h"
#include "GLTextureObject.h"
#include "Includes.h"
#include "ShaderProgram.h"
#include "glm/glm.hpp"
#include "samplerBindings.frag"

struct DiffuseMaterial {
    glm::vec4 diffuse = glm::vec4(1, 1, 1, 1);
    float specularCoefficient = 25;
    unsigned int isDiffuseTextureAvailable = 0;
};

enum TextureUnit : unsigned int { diffuseTexture = DIFFUSE_TEXTURE_TEXTURE_UNIT };

// TODO create better material system
class Material {
  public:
    Material() : id(NUM_MATERIALS++), diffuseMaterial() {
        materialBuffer.create(Buffer::BufferType::UniformBuffer, 0, sizeof(DiffuseMaterial), Buffer::UsageType::StaticDraw);
    }

    /**
     * @brief Sends the material data to the shader. THIS INCLUDES THE ASSOCIATED TEXTURES (if there are any).
     *
     */
    void connectMaterialDataToShader();

    /**
     * @brief Set the Diffuse Colour of the material
     *
     * @param _diffuse
     */
    void setDiffuseColour(glm::vec4 _diffuse);

    /**
     * @brief Set the Specular Coefficient object
     *
     * @param specular
     */
    void setSpecularCoefficient(float specular);

    /**
     * @brief assigns the given texture to the given unit, and binds it to the associated texture unit whenever this material
     * is bound. TAKES OWNERSHIP OF THE textureObject.
     *
     * @param textureUnit
     * @param textureObject
     */
    // TODO don't keep unique_ptr to textureobject here. Use a resource manager to store this shit
    void setTexture(TextureUnit textureUnit, std::unique_ptr<GLTextureObject> &textureObject);

    std::string name = "default";
    int id = 0;

  private:
    void updateBufferData() { materialBuffer.updateData(&(diffuseMaterial), sizeof(DiffuseMaterial), 0); }

    static unsigned NUM_MATERIALS;

    std::map<TextureUnit, std::unique_ptr<GLTextureObject> > textures;
    DiffuseMaterial diffuseMaterial;

    Buffer materialBuffer;
};

typedef std::map<int, Material> MaterialList;

class MaterialManager {
  public:
    void addMaterial(Material material);

  private:
    MaterialList materials;
};