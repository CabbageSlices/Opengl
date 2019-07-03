#include <map>
#include <string>

#include "glm/glm.hpp"

struct Material {

    Material() : id(NUM_MATERIALS++) {}
    Material(std::string _name, glm::vec4 _diffuse) : id(NUM_MATERIALS++), name(_name), diffuse(_diffuse) {}

    std::string name = "default";
    glm::vec4 diffuse = glm::vec4(1,1,1,1);
    int id = 0;
    
private:

    static unsigned NUM_MATERIALS;
};

typedef std::map<int, Material> MaterialList;

class MaterialManager {

public:
    void addMaterial(Material material);

private:

    MaterialList materials;
};