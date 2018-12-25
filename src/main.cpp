#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "glad/glad.h"
#include "SFML/OpenGL.hpp"
#include "SFML/Window.hpp"


#define TINYOBJLOADER_IMPLEMENTATION

#include "headers\Camera.h"
#include "headers/ShaderProgram.h"
#include "./headers/Shader.h"
#include "./headers/loadFileToString.h"
#include "./headers/loadFromObj.h"
#include "headers\Mesh.h"

using std::cout;
using std::endl;
using std::string;
using std::fstream;
using std::vector;

GLuint compileShader() {

	Shader shader;
	shader.loadAndCompileShader( Shader::Type::Vertex, "shaders/vertex.vert");
	
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	GLuint program = glCreateProgram();

	string vertexShaderSource = loadFileToString("shaders/vertex.vert");
	auto vertexShaderPtr = vertexShaderSource.c_str();

	string fragmentShaderSource = loadFileToString("shaders/fragment.frag");
	auto fragmentShaderPtr = fragmentShaderSource.c_str();

	glShaderSource(vertexShader, 1, &vertexShaderPtr, NULL);
	glCompileShader(vertexShader);

	glShaderSource(fragmentShader, 1, &fragmentShaderPtr, NULL);
	glCompileShader(fragmentShader);

	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return program;
}

GLuint createBuffer() {

	GLuint buffer = 0;

	glCreateBuffers(1, &buffer);
	glNamedBufferStorage(buffer, sizeof(float) * 3 * 10, nullptr, GL_DYNAMIC_STORAGE_BIT);

	return buffer;
}

int main(int argc, char const *argv[])
{
	
	//setup opengl context with version 4.5 core
	sf::Window window(sf::VideoMode(800, 600), "OpenGL",
		sf::Style::Default, sf::ContextSettings(24, 8, 4, 4, 5, sf::ContextSettings::Core));
	
	//activte the context
	window.setActive(true);

	//load the extension functions since sfml doesn't do it for you
	if(!gladLoadGL()) {
		cout << "Unable to load opengl" << endl;
		exit(-1);
	}

	bool isRunning = true;

	ShaderProgram program1;
	program1.loadAndCompileShaders({
		{Shader::Type::Vertex, "shaders/vertex.vert"},
		{Shader::Type::Fragment, "shaders/fragment.frag"}
	});

	program1.linkProgram();
	program1.useProgram();

	glPointSize(20);

	Mesh mesh;
	mesh.loadFromFile("untitled.obj");

	Camera camera;

	camera.setPosition(-3, 0, -3);
	camera.lookAt(0, 0, 0);

	program1.setUniform(ShaderProgram::WORLD_TO_CLIP_UNIFORM_LOCATION, false, camera.calculateWorldToClipMatrix());
	program1.setUniform(ShaderProgram::WORLD_TO_CAMERA_UNIFORM_LOCATION, false, camera.getWorldToCameraMatrix());
	program1.setUniform(1, {0.5, 0, 0, 0});

	while(isRunning) {

		sf::Event event;
		while (window.pollEvent(event)) {
			if(event.type == sf::Event::Closed)
				isRunning = false;
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		GLfloat clearColor[] = {0, 1, 0, 1};
		glClearBufferfv(GL_COLOR, 0, clearColor);
		mesh.render();
		window.display();
	}
	return 0;
}