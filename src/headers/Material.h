#include <map>
#include <string>
#include "glm/glm.hpp"

struct Material {

    std::string name = "default";
    glm::vec4 diffuse = glm::vec4(1,1,1,1);
};

typedef std::map<std::string, Material> MaterialList;

class MaterialManager {

public:
    void addMaterial(Material material);

private:

    MaterialList materials;
};