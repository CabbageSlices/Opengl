#pragma once
#include <map>
#include <string>

#include "glm/glm.hpp"
#include "ShaderProgram.h"
#include "Buffer.h"
#include "Includes.h"

//TODO create better material system
class Material {

public: 

    Material() : id(NUM_MATERIALS++) {
        materialBuffer.create(Buffer::BufferType::UniformBuffer, 0, sizeof(glm::vec4), Buffer::UsageType::StaticDraw);
    }

    void connectMaterialDataToShader() {
        materialBuffer.bindToTargetBindingPoint(MATERIAL_UNIFORM_BLOCK_BINDING_POINT);
    }

    void setDiffuse(glm::vec4 _diffuse) {
        diffuse = _diffuse;
        materialBuffer.updateData(&(diffuse), sizeof(glm::vec4), 0);
    }

    std::string name = "default";
    int id = 0;

private:

    glm::vec4 diffuse = glm::vec4(1,1,1,1);

    static unsigned NUM_MATERIALS;
    Buffer materialBuffer;
};

typedef std::map<int, Material> MaterialList;

class MaterialManager {

public:
    void addMaterial(Material material);

private:

    MaterialList materials;
};