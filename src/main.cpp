#define GLM_ENABLE_EXPERIMENTAL

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "glad/glad.h"
#include "SFML/OpenGL.hpp"
#include "SFML/Window.hpp"
#include "SFML\System.hpp"
#include "glm\gtc\quaternion.hpp"
#include "glm\ext\quaternion_trigonometric.hpp"
#include "glm\gtx\vector_angle.hpp"
#include "headers/PrintVector.hpp"
#include "glm/gtx/fast_trigonometry.hpp"


#define TINYOBJLOADER_IMPLEMENTATION

#include "headers\Camera.h"
#include "headers\CameraController.h"
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

	CameraController cameraController;
	// Camera camera;

	//at the top is error
	cameraController.camera.setPosition(-4, 0, 10);
	cameraController.camera.focusOnPoint(0, 0, 0);

	program1.setUniform(1, {0.5, 0, 0, 0});

	sf::Clock clock;
	clock.restart();

	glEnable(GL_DEPTH_TEST);

	while(isRunning) {

		sf::Event event;

		sf::Time elapsedTime = clock.getElapsedTime();
		clock.restart();
		float timeInSeconds = elapsedTime.asSeconds();

		while (window.pollEvent(event)) {
			if(event.type == sf::Event::Closed)
				isRunning = false;

			cameraController.handleEvent(event, timeInSeconds);
		}

		cameraController.handleInput(timeInSeconds);

		program1.setUniform(ShaderProgram::WORLD_TO_CLIP_UNIFORM_LOCATION, false, cameraController.camera.calculateWorldToClipMatrix());
		program1.setUniform(ShaderProgram::WORLD_TO_CAMERA_UNIFORM_LOCATION, false, cameraController.camera.getWorldToCameraMatrix());
	

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		GLfloat clearColor[] = {1, 1, 0, 1};
		glClearBufferfv(GL_COLOR, 0, clearColor);
		mesh.render();
		window.display();
	}
	return 0;
}