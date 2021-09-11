#pragma once
#include <tiny_obj_loader.h>

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

#include "./PrintVector.hpp"
#include "Material/Material.h"
#include "MeshData.h"
#include "glm\glm.hpp"

using std::cout;
using std::endl;
using std::make_shared;
using std::make_tuple;
using std::map;
using std::ostream;
using std::shared_ptr;
using std::string;
using std::tuple;
using std::vector;

// go to the given position in the array and combine 3 consequitive floats into a single vec3, and place the vec3 in the
// destination
inline void extractVector(const vector<float> &array, unsigned startPosition, glm::vec3 &destination) {
    destination = {array[startPosition], array[startPosition + 1], array[startPosition + 2]};
}

// go to the given position in the array and combine 2 consequitive floats into a single vec2, and place the vec2 in the
// destination
inline void extractVector(const vector<float> &array, unsigned startPosition, glm::vec2 &destination) {
    destination = {array[startPosition], array[startPosition + 1]};
}

// tiny obj attributes are an array of floats with a given number of elemetns per entry
// for example, a position attrbute has 3 elements per entry
// combines X number of elements into a single vectory <X1, X2 ... Xn> and places it into meshAttribute
// an entry indexed by i in the tinyobj array will be indexed by i in mesh attribute
template <typename T>
inline void tinyobjAttributeToMeshAttribute(const vector<float> &tinyobjAttribute, int elementsPerEntry,
                                            vector<T> &meshAttribute) {
    for (unsigned i = 0; i < tinyobjAttribute.size(); i += elementsPerEntry) {
        T entry;
        extractVector(tinyobjAttribute, i, entry);
        meshAttribute.push_back(entry);
    }
}

// tinyobj stores all atttributes as individual floats, store them as vectors so it's easy to index them
// normally if a mesh usees vertex index i, you need to access the attributes as i * 3 + 0,1,2
// which is annoying, so group them together in vectors so you can index them with just i
inline void tinyobjAttributesToMeshAttributes(const tinyobj::attrib_t &attributes, MeshAttributes &meshAttributes) {
    tinyobjAttributeToMeshAttribute(attributes.vertices, 3, meshAttributes.positions);
    tinyobjAttributeToMeshAttribute(attributes.normals, 3, meshAttributes.normals);

    if (attributes.texcoords.size() > 0) tinyobjAttributeToMeshAttribute(attributes.texcoords, 2, meshAttributes.texCoords);
}

// move the normals and positions around so that the position, normals, and texcoord have the same index.
// Opengl only allows 1 index buffer that has to index both position and normal, but obj files have multiple indices
// so for vertex 5, in tinyobj the position might be at index 3, normal at 8, and texcoord at 1
// need to change this so all 3 attributes have the same index, ex: 8
// reorganizes position, normals, and texcoords so they can all be indexed by the same number
// does NOT change: list of vertices that make up a face, the order of the faces being drawn
// meaning if face f1 is made of vertices v1,v2,v3 then it will still be made up of the same vertices
// AND if face f1 is the first face being drawn before this function is called, then afterwards it will still be drawn first
inline void tinyobjAttributeIndicesToOpenglIndices(const MeshAttributes &originalAttributes,
                                                   const std::vector<tinyobj::index_t> &originalIndices,
                                                   MeshAttributes &reorderedAttributes,
                                                   vector<unsigned int> &reorderedIndices) {
    typedef tuple<int, int, int> VertexIndex;
    map<VertexIndex, int> originalIndexToReorderedIndex;

    for (unsigned i = 0; i < originalIndices.size(); ++i) {
        tinyobj::index_t index = originalIndices[i];

        VertexIndex originalVertex = make_tuple(index.vertex_index, index.normal_index, index.texcoord_index);

        // the set {position, normal, texCoord} forms a unique vertex. If any attribute changes then it becomes a new vertex
        // if a vertex has already been indexed before use the index of the previous vertex instead of making a new entry
        if (originalIndexToReorderedIndex.count(originalVertex) != 0) {
            int combinedIndex = originalIndexToReorderedIndex[originalVertex];
            reorderedIndices.push_back(combinedIndex);
            continue;
        }

        glm::vec3 position = originalAttributes.positions[index.vertex_index];
        glm::vec3 normal = originalAttributes.normals[index.normal_index];

        // this combination is unique so it forms a new vertex, must add the vertex data for position, normal, and texcoord
        // and index this new vertex
        reorderedAttributes.positions.push_back(position);
        reorderedAttributes.normals.push_back(normal);

        // there is a texcoord so use it
        if (originalAttributes.texCoords.size() > 0) {
            reorderedAttributes.texCoords.push_back(originalAttributes.texCoords[index.texcoord_index]);
        }

        int reorderedIndex = reorderedAttributes.positions.size() - 1;
        reorderedIndices.push_back(reorderedIndex);
        originalIndexToReorderedIndex[originalVertex] = reorderedIndex;
    }
}

inline void tinyobjMaterialToCustomMaterial(const tinyobj::material_t &tinyobjMaterial, shared_ptr<Material> &material) {
    material->setAttribute("diffuseColor", glm::vec4(tinyobjMaterial.diffuse[0], tinyobjMaterial.diffuse[1],
                                                     tinyobjMaterial.diffuse[2], tinyobjMaterial.dissolve));
    material->setAttribute("specularCoefficient", (float)tinyobjMaterial.shininess);
    material->setAttribute<bool>("diffuseTexture_Provided", false);
}

inline void tinyobjMaterialsToCustomMaterials(const vector<tinyobj::material_t> &tinyobjMaterials,
                                              vector<shared_ptr<Material> > &convertedMaterials) {
    for (unsigned i = 0; i < tinyobjMaterials.size(); ++i) {
        shared_ptr<Material> convertedMaterial(new Material());
        tinyobjMaterialToCustomMaterial(tinyobjMaterials[i], convertedMaterial);
        convertedMaterials.push_back(convertedMaterial);
    }
}

// mesh is split into different sections by material, but it should be split by blueprint
// so each material needs to become a blueprint.
// and each of these blueprint will contain the asociated material as one of it's materials
inline void createBlueprintsFromMaterials(vector<shared_ptr<Material> > &convertedMaterials,
                                          vector<shared_ptr<RenderBlueprint> > &blueprints) {
    for (unsigned i = 0; i < convertedMaterials.size(); ++i) {
        shared_ptr<RenderBlueprint> defaultBlueprint = createDefaultBlueprint();
        defaultBlueprint->setMaterial("DiffuseMaterial", convertedMaterials[i]);

        blueprints.push_back(defaultBlueprint);
    }
}

// materials array MUST NOT BE EMPTY
inline void generateMaterialFaceMap(const vector<unsigned int> &reorderedIndices, const vector<int> &meshMaterialIndices,
                                    const vector<shared_ptr<Material> > &materials, map<int, FaceSet> &materialFaceMap) {
    // every 3 indices makes a face
    for (unsigned i = 0; i < reorderedIndices.size(); i += 3) {
        int faceId = i / 3;
        Face face{reorderedIndices[i], reorderedIndices[i + 1], reorderedIndices[i + 2]};

        int materialIndex = meshMaterialIndices[faceId];

        // face has no material assigned to it, assign it to whatever material is available
        if (materialIndex == -1) {
            materialIndex = materials.size() - 1;
        }

        // TO-DO handle missing materials
        assert((unsigned)materialIndex < materials.size());
        shared_ptr<Material> materialForFace = materials[materialIndex];

        // faceset does not exist for this material, create it
        if (materialFaceMap.count(materialForFace->getId()) == 0) {
            materialFaceMap[materialForFace->getId()] = FaceSet();
        }
        int id = materialForFace->getId();
        materialFaceMap[id].push_back(face);
    }
}

inline void generateBlueprintFaceMap(const vector<unsigned int> &reorderedIndices, const vector<int> &meshMaterialIndices,
                                     const vector<shared_ptr<RenderBlueprint> > &blueprints,
                                     map<int, FaceSet> &blueprintFaceMap) {
    // every 3 indices makes a face
    for (unsigned i = 0; i < reorderedIndices.size(); i += 3) {
        int faceId = i / 3;
        Face face{reorderedIndices[i], reorderedIndices[i + 1], reorderedIndices[i + 2]};

        int blueprintIndex = meshMaterialIndices[faceId];

        // face has no blueprint assigned to it, assign it to whatever blueprint is available
        if (blueprintIndex == -1) {
            blueprintIndex = blueprints.size() - 1;
        }

        // TO-DO handle missing blueprints
        assert((unsigned)blueprintIndex < blueprints.size());
        shared_ptr<RenderBlueprint> blueprintForFace = blueprints[blueprintIndex];

        int id = blueprintForFace->getId();

        // faceset does not exist for this blueprint, create it
        if (blueprintFaceMap.count(id) == 0) {
            blueprintFaceMap[id] = FaceSet();
        }

        blueprintFaceMap[id].push_back(face);
    }
}

/**
 * @brief Loads a mesh asset from the given filename. returns the mesh asset pointer, or null ptr if it failed
 *
 * @param objFileName filename of the obj file to load, assumes it's of type .obj, if there is no .obj extension, then .obj
 * will be appended to the filename
 * @return shared_ptr<MeshData> the loaded mesh data. null if loading failed
 */
inline shared_ptr<MeshData> loadFromObj(
    std::string objFileName /* MaterialManager &materialManager, std::vector<MeshData> &meshData*/) {
    shared_ptr<MeshData> meshData = std::make_shared<MeshData>();

    // load into tinyobj format
    // reorganize vertex data so that position/normal/etec can be indexed with a single index instead of a unique index per
    // attribute convert every material from tinyobj format to custom material format create meeshadata object return

    // add fileformat if it's missing
    if (objFileName.find(".obj") == std::string::npos) {
        objFileName.append(".obj");
    }

    // there could be multiple objects in the obj file but for now just load the first one
    tinyobj::attrib_t attrib;
    vector<tinyobj::shape_t> shapes;
    vector<tinyobj::material_t> materials;

    string warning;
    string error;
    bool result = tinyobj::LoadObj(&attrib, &shapes, &materials, &error, objFileName.c_str());

    if (!result) {
        cout << error << endl;
        return nullptr;
    }

    MeshAttributes originalAttributes;
    tinyobjAttributesToMeshAttributes(attrib, originalAttributes);

    // generate material
    vector<shared_ptr<Material> > convertedMaterials;
    tinyobjMaterialsToCustomMaterials(materials, convertedMaterials);

    // add default material so every mesh always has some material
    if (convertedMaterials.size() == 0) convertedMaterials.push_back(Material::createDefaultMaterial());

    // generate blueprints
    vector<shared_ptr<RenderBlueprint> > blueprints;
    createBlueprintsFromMaterials(convertedMaterials, blueprints);

    tinyobj::mesh_t mesh = shapes[0].mesh;
    MeshAttributes reorderedAttributes;
    vector<unsigned int> reorderedIndices;
    map<int, FaceSet> materialFaceMap;
    map<int, FaceSet> blueprintFaceMap;

    // reorganize vertex data so that position/normal/etec can be indexed with a single index instead of a unique index per
    // attribute
    tinyobjAttributeIndicesToOpenglIndices(originalAttributes, mesh.indices, reorderedAttributes, reorderedIndices);
    generateMaterialFaceMap(reorderedIndices, mesh.material_ids, convertedMaterials, materialFaceMap);
    generateBlueprintFaceMap(reorderedIndices, mesh.material_ids, blueprints, blueprintFaceMap);

    meshData->numIndices = reorderedIndices.size();
    meshData->attributes = reorderedAttributes;
    // meshData->materialFaceMap = materialFaceMap;
    // meshData->materials = convertedMaterials;
    meshData->blueprintFacemap = blueprintFaceMap;
    meshData->renderBlueprints = blueprints;

    return meshData;
}