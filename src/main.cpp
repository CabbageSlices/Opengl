#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "glad/glad.h"
#include "SFML/OpenGL.hpp"
#include "SFML/Window.hpp"

using std::cout;
using std::endl;
using std::string;
using std::fstream;
using std::vector;

string loadFileToString(const string & filename) {

	fstream file(filename);
	string fileAsString{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
	return fileAsString;
}

GLuint compileShader() {

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	GLuint program = glCreateProgram();

	string vertexShaderSource = loadFileToString("src/shaders/vertex.vert");
	auto vertexShaderPtr = vertexShaderSource.c_str();

	string fragmentShaderSource = loadFileToString("src/shaders/fragment.frag");
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

	GLuint shaderProgram = compileShader();
	glUseProgram(shaderProgram);

	GLuint vao;
	glCreateVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glPointSize(20);

	while(isRunning) {

		sf::Event event;
		while (window.pollEvent(event)) {
			if(event.type == sf::Event::Closed)
				isRunning = false;
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		GLfloat clearColor[] = {0, 1, 0, 1};
		glClearBufferfv(GL_COLOR, 0, clearColor);
		glDrawArrays(GL_POINTS, 0, 1);
		window.display();
	}
	return 0;
}