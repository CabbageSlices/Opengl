#pragma once

#include <iostream>
#include <glm\glm.hpp>

using std::ostream;


inline ostream& operator<< (ostream& stream, glm::vec3 val) {

	stream << "x: " << val.x << ", y: " << val.y << ", z: " << val.z;
	return stream;
}