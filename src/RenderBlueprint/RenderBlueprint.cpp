#include "./RenderBlueprint.h"

#include <vector>

#include "StringMethods.h"
#include "error_strings.h"
#include "nlohmann/json.hpp"

using nlohmann::json;
using std::string;
using std::vector;

unsigned RenderBlueprint::numBlueprintsCreated = 0;

const std::string RenderBlueprint::SCHEMA_MATERIALS_KEY = "materials";
const std::string RenderBlueprint::SCHEMA_SHADER_PROGRAMS_KEY = "shaderPrograms";
const std::string RenderBlueprint::SCHEMA_PASSES_KEY = "passes";

bool activateBlueprintForPass(const shared_ptr<RenderBlueprint> &blueprint, const RenderingPass &pass) {
    return activateBlueprintForPass(*blueprint, pass);
}

bool activateBlueprintForPass(const RenderBlueprint &blueprint, const RenderingPass &pass) {
    if (!blueprint.isRenderPassSupported(pass)) {
        return false;
    }

    // get the materials for the current pass, and the material uniform query for
    // the shader associated with this pass
    map<string, shared_ptr<Material>> materials = blueprint.getMaterialsForPass(pass);
    pair<string, shared_ptr<ShaderProgram>> program = blueprint.getShaderProgramForPass(pass);

    for (auto &materialPair : materials) {
        const string materialName = materialPair.first;
        const string shaderProgramName = program.first;
        shared_ptr<MaterialPropertiesQueryInfo> queryInfo =
            blueprint.getMaterialPropertiesQueryForShaderProgram(shaderProgramName, materialName);

        shared_ptr<Material> material = materialPair.second;

        material->setQueryInfo(queryInfo, true);

        material->activate();
    }

    program.second->useProgram();

    return true;
}

shared_ptr<RenderBlueprint> createDefaultBlueprint() {
    const string schema = loadFileToString("blueprints/default.json");
    shared_ptr<RenderBlueprint> blueprint(new RenderBlueprint(schema));
    return blueprint;
}

RenderBlueprint::RenderBlueprint(const string &blueprintJsonSchema) : id(numBlueprintsCreated++) {
    parseJsonSchema(blueprintJsonSchema);
}

bool RenderBlueprint::isRenderPassSupported(const RenderingPass &pass) const {
    return shaderProgramToUseForRenderPass.count(renderPassNames[pass]) > 0;
}

map<string, shared_ptr<Material>> RenderBlueprint::getMaterialsForPass(const RenderingPass &pass) const {
    map<string, shared_ptr<Material>> materialsForThisPass;
    const string passString = renderPassNames[pass];
    const string &shaderProgramNameForPass = shaderProgramToUseForRenderPass.at(passString);
    auto &propertiesQueryInfo = materialPropertiesQueryInfoForShaderProgram.at(shaderProgramNameForPass);

    for (auto &queryPair : propertiesQueryInfo) {
        const string &materialName = queryPair.first;
        materialsForThisPass[materialName] = materials.at(materialName);
    }

    return materialsForThisPass;
}

std::pair<std::string, std::shared_ptr<ShaderProgram>> RenderBlueprint::getShaderProgramForPass(
    const RenderingPass &pass) const {
    const string &passString = renderPassNames[pass];
    const string &programName = shaderProgramToUseForRenderPass.at(passString);
    shared_ptr<ShaderProgram> program = shaderPrograms.at(programName);

    return {programName, program};
}

shared_ptr<MaterialPropertiesQueryInfo> RenderBlueprint::getMaterialPropertiesQueryForShaderProgram(
    const string &shaderProgramName, const string &materialName) const {
    auto &propertiesQueryInfo = materialPropertiesQueryInfoForShaderProgram.at(shaderProgramName);

    if (propertiesQueryInfo.count(materialName) == 0) {
        throw "shader: \'" + shaderProgramName + "\' does not use the material \'" + materialName + "\'";
    }
    return propertiesQueryInfo.at(materialName);
}

void RenderBlueprint::setMaterial(const std::string &materialName, const std::shared_ptr<Material> &material) {
    if (materials.count(materialName) == 0) {
        throw "Material \'" + materialName + "\' not part of the blueprint, cannot set material";
    }
    materials[materialName] = material;
    material->requireBufferUpdate();
}

const std::shared_ptr<Material> RenderBlueprint::getMaterial(const std::string &materialName) {
    if (materials.count(materialName) == 0) {
        throw "Material \'" + materialName + "\' not part of the blueprint, cannot set material";
    }
    return materials[materialName];
}

void RenderBlueprint::parseJsonSchema(const string &blueprintJsonSchema) {
    json schema = json::parse(blueprintJsonSchema);

    // check for shader programs
    const auto &shaderProgramsSchema = schema.find(SCHEMA_SHADER_PROGRAMS_KEY);

    if (shaderProgramsSchema == schema.end()) {
        throw string("shader program definitions not found");
    }

    const auto &passes = schema.find(SCHEMA_PASSES_KEY);

    if (passes == schema.end()) {
        throw string("render passes not found");
    }

    // check for materials
    const auto &materialsSchema = schema.find(SCHEMA_MATERIALS_KEY);

    // materials exist
    if (materialsSchema != schema.end()) {
        // should be array of strings
        vector<string> materialNames = materialsSchema->get<vector<string>>();

        createMaterials(materialNames);
    }

    parseShaderPrograms(shaderProgramsSchema.value());

    parseRenderPasses(passes.value(), shaderPrograms, materials);
}

void RenderBlueprint::parseShaderPrograms(const nlohmann::json &shaderProgramsSchema) {
    for (auto &programPair : shaderProgramsSchema.items()) {
        const string shaderProgramName = programPair.key();
        const auto &shaderProgramDefinition = programPair.value();

        map<Shader::Type, string> shadersToLoad;

        for (const auto &shaderType : Shader::SHADER_TYPE_TO_STRING) {
            const string &shaderTypeString = shaderType.second;

            const auto &shaderDefinition = shaderProgramDefinition.find(shaderTypeString);

            // this type of shader doesn't exist so don't load it
            if (shaderDefinition == shaderProgramDefinition.end()) {
                continue;
            }

            shadersToLoad[shaderType.first] = shaderDefinition.value().get<string>();
        }

        bool usingDefaultShaderDirectory = (shaderProgramDefinition.contains("usingDefaultShaderDirectory")
                                                ? shaderProgramDefinition["usingDefaultShaderDirectory"].get<bool>()
                                                : true);

        // no valid shader types found
        if (shadersToLoad.size() == 0) {
            throw shaderProgramName + " does not have any valid shader files in blueprint.";
        }

        shared_ptr<ShaderProgram> program(new ShaderProgram());
        program->loadAndCompileShaders(shadersToLoad, usingDefaultShaderDirectory);
        program->linkProgram();

        shaderPrograms[shaderProgramName] = program;
    }
}

void RenderBlueprint::parseRenderPasses(const nlohmann::json &renderPasses,
                                        const map<string, shared_ptr<ShaderProgram>> &shaderPrograms,
                                        const map<string, shared_ptr<Material>> &materials) {
    for (const auto &passes : renderPasses.items()) {
        const string &passName = passes.key();
        const auto &passSchema = passes.value();
        const string &programName = passSchema["shaderProgram"];

        shaderProgramToUseForRenderPass[passName] = programName;

        const auto &materialNamesJson = passSchema.find("materials");

        if (shaderPrograms.count(programName) == 0) {
            throw programName + " is used as a shader in a render pass but isn't defined in the shader definitions";
        }

        // creating a default empty query collection for this program, if it doesn't exist.
        // don't assign to it because that would override the previous one
        // need this to prevent an exception
        materialPropertiesQueryInfoForShaderProgram[programName];

        if (materialNamesJson == passSchema.end()) {
            continue;
        }

        const auto &materialNames = materialNamesJson.value().get<vector<string>>();

        for (const auto &materialName : materialNames) {
            if (materials.count(materialName) == 0) {
                throw materialName + " material used by render pass but isnt declared in the materials section";
            }
            // load the query info for all materials for this shader
            shared_ptr<ShaderProgram> shaderProgram = shaderPrograms.at(programName);

            shared_ptr<MaterialPropertiesQueryInfo> queryInfo(new MaterialPropertiesQueryInfo());
            queryInfo->queryBlockData(shaderProgram, materialName);

            materialPropertiesQueryInfoForShaderProgram[programName][materialName] = queryInfo;
        }
    }
}

void RenderBlueprint::createMaterials(const vector<string> &materialNames) {
    for (const string &name : materialNames) {
        shared_ptr<Material> material(new Material());
        materials[name] = material;
    }
}
