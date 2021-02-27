#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "Material/Material.h"
#include "glm\glm.hpp"

struct MeshAttributes {
    void clear() {
        positions.clear();
        normals.clear();
        texCoords.clear();
    }

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;
};

// 3 indices to make up a face
// each index should refer to position, normal, and texcoords SIMULTANEOUSLY
/*that is face 1 =  {
        vertex1: {
                position: positions[i1]
                normal: normals[i1]
                texCoord: texCoords[i1]
        },
        vertex2: {
                ...[i2]
        },
}*/
struct Face {
    unsigned i1, i2, i3;
};

// typedef glm::uvec3 Face;

// collection of faces, this way all indices are consequtive and laid out in memory linearly so you can send them to opengl
typedef std::vector<Face> FaceSet;

// mesh data works as follows:
/**
 * Entire mesh is split into groups of faces that share a single material, this group of faces is called a faceset.
 * each faceset is referenced by it's material id, so to get a group of faces you use the material ID.
 * A face has 3 verticies, so once you have a face set, you can index each individual face in the set.
 * A face is composed of 3 indices, 1 for each vertex. The index will simultaneously index into the postiion, normal, and
 * texcoord array. So a face with indices 0, 1, 2 means it uses position/normal/texcoord [0], [1], and [2] All mesh attribtue
 * data is stored in the MeshAttributes object. If the texcoord or normal array is empty then the mesh does not contain any
 * texcoord or normal data, only position It's up to the user to make sure they don't utilize the mepty materials.*/
struct MeshData {
    void clear() {
        attributes.clear();

        materialFaceMap.clear();
    }

    MeshAttributes attributes;

    // map of material name to the faces that use that material
    // maps material id to faceset
    std::map<int, FaceSet> materialFaceMap;

    // total number of indices so you don't have to iterate through the facemap
    int numIndices;

    std::vector<std::shared_ptr<Material> > materials;
};