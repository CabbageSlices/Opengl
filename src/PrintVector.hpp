#pragma once

#include <glm\glm.hpp>
#include <iostream>

using std::ostream;

inline ostream& operator<<(ostream& stream, glm::vec3 val) {
    stream << "x: " << val.x << ", y: " << val.y << ", z: " << val.z;
    return stream;
}

inline ostream& operator<<(ostream& stream, glm::vec2 val) {
    stream << "x: " << val.x << ", y: " << val.y;
    return stream;
}