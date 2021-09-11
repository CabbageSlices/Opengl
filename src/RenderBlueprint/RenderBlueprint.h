#ifndef __RENDERBLUEPRINT_H__
#define __RENDERBLUEPRINT_H__

#include <map>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>

#include "GraphicsWrapper.h"
#include "Material/Material.h"
#include "Material/MaterialPropertiesQueryInfo.h"
#include "ShaderProgram/ShaderProgram.h"

// material name, mateiral properties query
typedef std::map<std::string, std::shared_ptr<MaterialPropertiesQueryInfo>> PropertiesQueryCollection;

class RenderBlueprint {
  public:
    RenderBlueprint() : id(numBlueprintsCreated++){};
    RenderBlueprint(const std::string &blueprintJsonSchema);

    static const std::string SCHEMA_MATERIALS_KEY;
    static const std::string SCHEMA_SHADER_PROGRAMS_KEY;
    static const std::string SCHEMA_PASSES_KEY;

    bool isRenderPassSupported(const RenderingPass &pass) const;
    std::map<std::string, std::shared_ptr<Material>> getMaterialsForPass(const RenderingPass &pass) const;
    std::pair<std::string, std::shared_ptr<ShaderProgram>> getShaderProgramForPass(const RenderingPass &pass) const;
    std::shared_ptr<MaterialPropertiesQueryInfo> getMaterialPropertiesQueryForShaderProgram(
        const std::string &shaderProgramName, const std::string &materialName) const;

    const std::map<std::string, PropertiesQueryCollection> &getMaterialPropertiesQueryInfos() const {
        return materialPropertiesQueryInfoForShaderProgram;
    }

    unsigned getId() const { return id; }

    void setMaterial(const std::string &materialName, const std::shared_ptr<Material> &material);
    const std::shared_ptr<Material> getMaterial(const std::string &materialName);

  private:
    void parseJsonSchema(const std::string &blueprintJsonSchema);

    // read shader programs, load shaders from disk, and compile/link them to shader programs
    void parseShaderPrograms(const nlohmann::json &shaderPrograms);

    // read the render passes, assign shader programs to their respective passes,
    // generate material query info for each material used by hte shader programs
    void parseRenderPasses(const nlohmann::json &renderPasses,
                           const std::map<std::string, std::shared_ptr<ShaderProgram>> &shaderPrograms,
                           const std::map<std::string, std::shared_ptr<Material>> &materials);

    // generate the materials using the given names. The laoded materials will have no query info
    void createMaterials(const std::vector<std::string> &materialNames);

    unsigned id;
    static unsigned numBlueprintsCreated;

    std::map<std::string, std::shared_ptr<Material>> materials;
    std::map<std::string, std::shared_ptr<ShaderProgram>> shaderPrograms;

    // pair renderPassName shaderProgramName
    std::map<std::string, std::string> shaderProgramToUseForRenderPass;

    // pair shader program name, material query info
    // for a given shader, get all the mateiral properties query info for all mateirals that is used by that shader
    std::map<std::string, PropertiesQueryCollection> materialPropertiesQueryInfoForShaderProgram;
};

// activate shader, material, uniforms, etc for the given pass
// returns true if the given pass is supported by this blueprint
extern bool activateBlueprintForPass(const std::shared_ptr<RenderBlueprint> &blueprint, const RenderingPass &pass);

// activate shader, material, uniforms, etc for the given pass
// returns true if the given pass is supported by this blueprint
extern bool activateBlueprintForPass(const RenderBlueprint &blueprint, const RenderingPass &pass);

extern std::shared_ptr<RenderBlueprint> createDefaultBlueprint();

#endif  // __RENDERBLUEPRINT_H__