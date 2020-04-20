#ifndef TESTING
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "glad/glad.h"
#include "SFML/OpenGL.hpp"
#include "SFML/Window.hpp"
#include "SFML\System.hpp"
#include "PrintVector.hpp"
#include "Entity/Entity.h"
#include <memory>

#include "Camera/Camera.h"
#include "Camera/CameraController.h"
#include "ShaderProgram.h"
#include "./Shader.h"
#include "./StringMethods.h"
#include "./loadFromObj.h"
#include "Includes.h"
#include "MeshRenderer.h"
#include "components/MeshRendererComponent.h"
#include "Lights/LightManager.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using std::cout;
using std::endl;
using std::string;
using std::fstream;
using std::vector;

TEST(lolz, lol2) {
	ASSERT_EQ(5,5);
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
		{Shader::Type::Vertex, "vertex.vert"},
		{Shader::Type::Fragment, "fragment.frag"}
	});

	program1.linkProgram();
	program1.useProgram();

	glPointSize(20);

	std::shared_ptr<MeshData> cubeMeshData = loadFromObj("cube.obj");
	std::shared_ptr<MeshRendererComponent> cubeMeshRendererComponent(new MeshRendererComponent(cubeMeshData));
	
	Entity cube;
	cube.addComponent(cubeMeshRendererComponent);
	cube.setRotation(45, 45, 0);
	// MeshRenderer mesh(cubeMeshData);

	// MeshAsset cubeAsset = AssetManager.loadObj("smoothsphere.obj");
	// MeshComponent cubeMesh(cubeAsset);

	// Entity cube(position, rotation, scale);
	// cube.addComponent(cubeMesh);


	CameraController cameraController({0, 0, 5}, {0, 0, 0});

	LightManager lightManager;
	lightManager.createDirectionalLight({0, -1, 0, 0}, {1,1,1,1});
	lightManager.createDirectionalLight({0, 1, 0, 0}, {1,1,1,1});
	lightManager.createDirectionalLight({0, -1, 0.5, 0}, {1,0,0,1});
	lightManager.createDirectionalLight({0, -1, -0.5, 0}, {0,1,0,1});
	lightManager.createPointLight({0, 2, 0, 1}, {1,0,0,1}, 3);
	lightManager.createPointLight({0, -2, 0, 1}, {0,1,0,1}, 3);
	lightManager.createPointLight({2, 0, 0, 1}, {0,0,1,1}, 3);
	lightManager.createPointLight({0, 0, 2, 1}, {1,0,1,1}, 3);
	lightManager.createPointLight({-2, 0, 0, 1}, {1,1,0,1}, 3);
	lightManager.createPointLight({0, 0, -2, 1}, {0,1,1,1}, 3);

	lightManager.connectLightDataToShader();

	vector<DirectionalLight> directionalLightsForUniform = lightManager.getDirectionalLights();
	vector<PointLight> pointLightsForUniform = lightManager.getPointLights();

	program1.setUniform(1, {0.5, 0, 0, 0});

	sf::Clock clock;
	clock.restart();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glDepthFunc(GL_LEQUAL);
	glBlendFunc(GL_ONE, GL_ONE);
	glClearColor(0, 0, 0, 0);

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

		program1.setUniform(WORLD_TO_CLIP_UNIFORM_BUFFER_LOCATION, false, cameraController.getCamera().calculateWorldToClipMatrix());
		program1.setUniform(WORLD_TO_CAMERA_UNIFORM_BUFFER_LOCATION, false, cameraController.getCamera().getWorldToCameraMatrix());
	
		cube.rotate(glm::vec3(0, 1, 0), elapsedTime.asSeconds() * 50);
		cube.rotate(glm::vec3(1, 0, 0), elapsedTime.asSeconds() * 10);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDisable(GL_BLEND); //first pass disable blend because when you render, if a triangle is rendered in the background, and then something renders on top of it, the background and 
		//the triangle on top will be blended together.

		lightManager.sendBatchToShader(0);
		cube.render();

		//additoinal passes, enable blend
		glEnable(GL_BLEND); 
		for(int i = 1; i < lightManager.getBatchCount(); ++i) {

			lightManager.sendBatchToShader(i);
			cube.render();
		}
		window.display();
	}
	return 0;
}
#endif