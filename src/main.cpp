#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "./Shader.h"
#include "./StringMethods.h"
#include "./loadFromObj.h"
#include "Camera/Camera.h"
#include "Camera/CameraController.h"
#include "Entity/Entity.h"
#include "GLTextureObject.h"
#include "Includes.h"
#include "Lights/LightManager.h"
#include "MeshRenderer.h"
#include "PrintVector.hpp"
#include "SFML/OpenGL.hpp"
#include "SFML/Window.hpp"
#include "SFML\System.hpp"
#include "ShaderProgram.h"
#include "TextureResource/ImageTextureResource.h"
#include "components/MeshRendererComponent.h"
#include "glad/glad.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using std::cout;
using std::endl;
using std::fstream;
using std::string;
using std::vector;

// TODO make a log to file system to log stuff to a file
// log out all mesh buffers, camera buffers, and compare to when c_str() is called
// to determine why the fuck stbi_load fucks things up.
#define FLOG cout << __FILE__ << ":" << __LINE__ << ":: "

int main() {
    // setup opengl context with version 4.6 core
    sf::Window window(sf::VideoMode(800, 600), "OpenGL", sf::Style::Default,
                      sf::ContextSettings(24, 8, 4, 4, 6, sf::ContextSettings::Core));

    // activte the context
    window.setActive(true);

    // load the extension functions since sfml doesn't do it for you
    if (!gladLoadGL()) {
        cout << "Unable to load opengl" << endl;
        exit(-1);
    }
    {  // temp scope to ensure all destrucotrs are called before opengl is erased
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        stbi_set_flip_vertically_on_load(true);

        bool isRunning = true;

        ShaderProgram program1;
        program1.loadAndCompileShaders({{Shader::Type::Vertex, "vertex.vert"}, {Shader::Type::Fragment, "fragment.frag"}});

        ShaderProgram texturedRect;
        texturedRect.loadAndCompileShaders(
            {{Shader::Type::Vertex, "texturedRect.vert"}, {Shader::Type::Fragment, "texturedRect.frag"}});

        texturedRect.linkProgram();
        texturedRect.useProgram();

        program1.linkProgram();
        program1.useProgram();

        glPointSize(20);

        std::shared_ptr<MeshData> cubeMeshData = loadFromObj("cube.obj");
        std::shared_ptr<MeshRendererComponent> cubeMeshRendererComponent(new MeshRendererComponent(cubeMeshData));

        Entity cube;
        cube.addComponent(cubeMeshRendererComponent);

        CameraController cameraController({0, 0, 5}, {0, 0, 0});

        LightManager lightManager;
        lightManager.createDirectionalLight({0, -1, 0, 0}, {1, 1, 1, 1});
        lightManager.createDirectionalLight({0, 1, 0, 0}, {1, 1, 1, 1});
        lightManager.createDirectionalLight({0, -1, 0.5, 0}, {1, 0, 0, 1});
        lightManager.createDirectionalLight({0, -1, -0.5, 0}, {0, 1, 0, 1});
        lightManager.createPointLight({0, 2, 0, 1}, {1, 0, 0, 1}, 3);
        lightManager.createPointLight({0, -2, 0, 1}, {0, 1, 0, 1}, 3);
        lightManager.createPointLight({2, 0, 0, 1}, {0, 0, 1, 1}, 3);
        lightManager.createPointLight({0, 0, 2, 1}, {1, 0, 1, 1}, 3);
        lightManager.createPointLight({-2, 0, 0, 1}, {1, 1, 0, 1}, 3);
        lightManager.createPointLight({0, 0, -2, 1}, {0, 1, 1, 1}, 3);

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
        glEnable(GL_CULL_FACE);
        glClearColor(0, 0, 0, 0);

        int width = 0, height = 0, channels = 0;

        unsigned char *imageData = stbi_load("./images/small.png", &width, &height, &channels, 0);

        if (!imageData) {
            cout << "failed to load image" << endl;
        } else {
            cout << width << " " << height << " " << channels << endl;
        }

        ImageTextureResource image("./images/small.png");
        auto textureObject = image.createGLTextureObject(0);
        textureObject->bindToTextureUnit(0);

        // ImageTextureResource image()

        // TextureResource::CreateGLTextureObject(textureResource);

        // stores image data as texture data in ram
        // unrelated to opengl shit
        // Texture textureResource = Texture::loadFromFile("./images/grasslands.jpg", desiredChannels);
        // GLTexture textureObject;

        // //create the texture
        // //create opgnel object, setup storage, load data to gpu
        // textureObject.create(textureResource, numMipmappingLevels);

        // textureObject.bindToTextureUnit(unit_number);

        // create some kinda 2d texture
        // GLuint texture;
        // glCreateTextures(GL_TEXTURE_2D, 1, &texture);
        // glTextureStorage2D(texture, 1, GL_RGBA8, width, height);

        // glTextureSubImage2D(texture, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
        // glBindTextureUnit(0, texture);

        cout << "LOADED BUT GRAHPICS R FUCKED???" << endl;

        // TODO load image as texture
        GLuint vao;
        glGenVertexArrays(1, &vao);

        while (isRunning) {
            sf::Event event;

            sf::Time elapsedTime = clock.getElapsedTime();
            clock.restart();
            float timeInSeconds = elapsedTime.asSeconds();

            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) isRunning = false;

                cameraController.handleEvent(event, timeInSeconds);
            }

            cameraController.handleInput(timeInSeconds);

            program1.setUniform(WORLD_TO_CLIP_UNIFORM_BUFFER_LOCATION, false,
                                cameraController.getCamera().calculateWorldToClipMatrix());
            program1.setUniform(WORLD_TO_CAMERA_UNIFORM_BUFFER_LOCATION, false,
                                cameraController.getCamera().getWorldToCameraMatrix());

            // cube.rotate(glm::vec3(0, 1, 0), elapsedTime.asSeconds() * 50);
            // cube.rotate(glm::vec3(1, 0, 0), elapsedTime.asSeconds() * 10);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glDisable(GL_BLEND);  // first pass disable blend because when you render, if a triangle is rendered in the
                                  // background, and then something renders on top of it, the background and
            // the triangle on top will be blended together.

            lightManager.sendBatchToShader(0);
            cube.render();

            // additoinal passes, enable blend
            glEnable(GL_BLEND);
            for (int i = 1; i < lightManager.getBatchCount(); ++i) {
                lightManager.sendBatchToShader(i);
                cube.render();
            }

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glBindVertexArray(vao);
            texturedRect.useProgram();
            glDrawArrays(GL_TRIANGLES, 0, 6);

            window.display();
        }
    }
    return 0;
}