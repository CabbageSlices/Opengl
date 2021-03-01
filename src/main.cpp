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
#include "GLFrameBufferObject.h"
#include "GLTextureObject.h"
#include "Includes.h"
#include "Lights/LightManager.h"
#include "MeshRenderer.h"
#include "PrintVector.hpp"
#include "SFML/OpenGL.hpp"
#include "SFML/Window.hpp"
#include "SFML\System.hpp"
#include "ShaderProgram/shaderprogram.h"
#include "TextureResource/ImageTextureResource.h"
#include "components/MeshRendererComponent.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using std::cout;
using std::endl;
using std::fstream;
using std::string;
using std::vector;

void GLAPIENTRY debugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                                     const GLchar* message, const void* userParam) {
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, source = 0x%x, severity = 0x%x, message = %s\n",
            (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, source, severity, message);
}

// TODO make a log to file system to log stuff to a file
// log out all mesh buffers, camera buffers, and compare to when c_str() is called
// to determine why the fuck stbi_load fucks things up.
#define FLOG cout << __FILE__ << ":" << __LINE__ << ":: "

int main() {
    cout << sizeof(bool) << endl;
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

        // During init, enable debug output
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(debugMessageCallback, 0);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, 0, GL_FALSE);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_LOW, 0, 0, GL_FALSE);

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

        std::shared_ptr<MeshData> cubeMeshData = loadFromObj("smoothsphere.obj");
        std::shared_ptr<MeshRendererComponent> cubeMeshRendererComponent(new MeshRendererComponent(cubeMeshData));

        Entity cube;
        cube.addComponent(cubeMeshRendererComponent);

        CameraController cameraController({0, 0, 5}, {0, 0, 0});

        LightManager lightManager;
        lightManager.createDirectionalLight({0, -1, 0, 0}, {1, 1, 1, 1});
        lightManager.createDirectionalLight({0, 0, 1, 0}, {1, 1, 1, 1});
        lightManager.createDirectionalLight({0, 0, -1, 0}, {1, 1, 1, 1});
        lightManager.createPointLight({5, 0, 0, 1}, {0.7, 0.7, 0.7, 0.7}, 10);

        lightManager.connectLightDataToShader();

        vector<DirectionalLight> directionalLightsForUniform = lightManager.getDirectionalLights();
        vector<PointLight> pointLightsForUniform = lightManager.getPointLights();

        sf::Clock clock;
        clock.restart();

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glDepthFunc(GL_LEQUAL);
        glBlendFunc(GL_ONE, GL_ONE);
        glEnable(GL_CULL_FACE);
        glClearColor(0, 0, 0, 0);

        int width = 0, height = 0, channels = 0;

        unsigned char* imageData = stbi_load("./images/small.png", &width, &height, &channels, 0);

        if (!imageData) {
            cout << "failed to load image" << endl;
        }

        // TODO load image as texture
        GLuint vao;
        glGenVertexArrays(1, &vao);

        GLTextureObject framebufferTexture(TextureType::TEXTURE_2D);
        framebufferTexture.create(1, SizedColourFormats::RGBA_8, 800, 600, ColourFormats::RGBA, DataType::FLOAT, 0);

        // glNamedFramebufferTexture(framebuffer, GL_COLOR_ATTACHMENT0, framebufferTexture.getTextureObject(), 0);
        // glNamedFramebufferDrawBuffer(framebuffer, GL_COLOR_ATTACHMENT0);

        GLFramebufferObject fbo;
        fbo.create();
        fbo.attachTexture(FramebufferColorAttachment::COLOR_ATTACHMENT1, framebufferTexture, 0);

        // fbo.enableDrawingToAttachments({FramebufferColorAttachment::COLOR_ATTACHMENT1});

        // loads the mesh data, materials, textures, and creates the shaders
        // ResourceManager::getInstance().loadFromObj("mesh.obj");

        // ResourceManager::loadFromObj("mesh.obj");
        // weak_ptr<MeshData> cubeMesh = ResourceManager::get<MeshData>("cube");
        // cubeMesh.setMaterial(ResourceManager::get<Material>())

        // ResourceManager::registerLoader("obj", loadFromObj);
        // vector<weak_ptr<meshData>> meshs = ResourceManager::load("blah.obj");
        // meshs[0]->materials[0]->setShader(program1);

        Buffer cameraBuffer;
        GLuint bufferSize = sizeof(glm::mat4) * 2 + sizeof(glm::vec4);
        cameraBuffer.create(Buffer::BufferType::UniformBuffer, 0, bufferSize, Buffer::UsageType::StreamDraw);
        cameraBuffer.bindToTargetBindingPoint(UNIFORM_TRANSFORM_MATRICES_BLOCK_BINDING_POINT);

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

            cameraBuffer.updateData(glm::value_ptr(cameraController.getCamera().calculateWorldToClipMatrix()),
                                    sizeof(glm::mat4), 0);
            cameraBuffer.updateData(glm::value_ptr(cameraController.getCamera().getWorldToCameraMatrix()), sizeof(glm::mat4),
                                    sizeof(glm::mat4));
            cameraBuffer.updateData(glm::value_ptr(cameraController.getCamera().getPosition()), sizeof(glm::vec4),
                                    sizeof(glm::mat4) * 2);

            // glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
            fbo.bindToTarget(FramebufferTarget::FRAMEBUFFER);

            glDisable(GL_BLEND);  // first pass disable blend because when you render, if a triangle is rendered in the
                                  // background, and then something renders on top of it, the background and
            // the triangle on top will be blended together.
            program1.useProgram();

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            lightManager.sendBatchToShader(0);

            cube.render();

            // // additoinal passes, enable blend
            glEnable(GL_BLEND);
            for (int i = 1; i < lightManager.getBatchCount(); ++i) {
                lightManager.sendBatchToShader(i);
                cube.render();
            }

            // glBindVertexArray(vao);
            // glDrawArrays(GL_TRIANGLES, 0, 3);

            // GLenum err = glGetError();
            // if (err != GL_NO_ERROR) {
            //     cout << "ERROR: " << err << endl;
            // }

            glDisable(GL_BLEND);

            // glBindFramebuffer(GL_FRAMEBUFFER, 0);
            GLFramebufferObject::clearFramebufferAtTarget(FramebufferTarget::FRAMEBUFFER);
            framebufferTexture.bindToTextureUnit(0);
            GLfloat black[] = {0, 0, 0, 0};
            glClearBufferfv(GL_COLOR, 0, black);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glBindVertexArray(vao);
            texturedRect.useProgram();
            glDrawArrays(GL_TRIANGLES, 0, 6);

            window.display();
        }
    }
    return 0;
}