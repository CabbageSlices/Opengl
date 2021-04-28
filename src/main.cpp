#include <exception>
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
    std::set_terminate([]() {
        cout << "Unhandled exception" << endl;
        std::abort();
    });
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

        ShaderProgram shadowMapGeneratorProgram;
        shadowMapGeneratorProgram.loadAndCompileShaders(
            {{Shader::Type::Vertex, "shadowMapPass.vert"}, {Shader::Type::Fragment, "shadowMapPass.frag"}});
        shadowMapGeneratorProgram.linkProgram();

        shared_ptr<ShaderProgram> shadowMapGeneratorProgram1(new ShaderProgram());
        shadowMapGeneratorProgram1->loadAndCompileShaders(
            {{Shader::Type::Vertex, "shadowMapPass.vert"}, {Shader::Type::Fragment, "shadowMapPass.frag"}});
        shadowMapGeneratorProgram1->linkProgram();

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

        std::shared_ptr<MeshRendererComponent> secondMeshRendererComponent(new MeshRendererComponent(cubeMeshData));
        Entity secondEntity;
        secondEntity.addComponent(secondMeshRendererComponent);
        secondEntity.setPosition({-1, -2, 0});

        CameraController cameraController({0, 0, 5}, {0, 0, 0});

        LightManager lightManager;
        lightManager.createDirectionalLight({1, 20, 0, 1}, {-0.2, -1, 0, 0}, {1, 1, 1, 1});
        // lightManager.createDirectionalLight({0, 0, 1, 0}, {1, 1, 1, 1});
        // lightManager.createDirectionalLight({0, 0, -1, 0}, {1, 1, 1, 1});
        // lightManager.createPointLight({5, 0, 0, 1}, {0.7, 0.7, 0.7, 0.7}, 10);

        lightManager.connectLightDataToShader();

        sf::Clock clock;
        clock.restart();

        glEnable(GL_DEPTH_TEST);
        glClearDepth(1.0);
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

        GLTextureObject depthTexture(TextureType::TEXTURE_2D);
        depthTexture.create(1, TextureInternalStorageFormat::DEPTH_COMPONENT32, 1024, 1024, PixelDataFormat::DEPTH_COMPONENT,
                            DataType::FLOAT, 0);

        GLFramebufferObject fbo;
        fbo.create();
        fbo.attachTexture(FramebufferNonColorAttachment::DEPTH_ATTACHMENT, depthTexture, 0);

        depthTexture.setParameter(TextureParameterType::MIN_FILTER, TextureFilteringModes::LINEAR);
        depthTexture.setParameter(TextureParameterType::MAG_FILTER, TextureFilteringModes::LINEAR);

        depthTexture.setParameter(TextureParameterType::TEXTURE_WRAP_S, TextureWrapModes::CLAMP_TO_EDGE);
        depthTexture.setParameter(TextureParameterType::TEXTURE_WRAP_T, TextureWrapModes::CLAMP_TO_EDGE);

        glNamedFramebufferDrawBuffer(fbo.getFramebufferObject(), GL_NONE);

        // loads the mesh data, materials, textures, and creates the shaders
        // ResourceManager::getInstance().loadFromObj("mesh.obj");

        // ResourceManager::loadFromObj("mesh.obj");
        // weak_ptr<MeshData> cubeMesh = ResourceManager::get<MeshData>("cube");
        // cubeMesh.setMaterial(ResourceManager::get<Material>())

        // ResourceManager::registerLoader("obj", loadFromObj);
        // vector<weak_ptr<meshData>> meshs = ResourceManager::load("blah.obj");
        // meshs[0]->materials[0]->setShader(program1);

        // Material testMat(MaterialPropertiesQueryInfo);
        // MaterialCollection matCollection;
        // matCollection.addMaterial(RenderingPass::REGULAR_PASS, testMat);
        // matCollection.addMaterial(RenderingPass::DEPTH_PASS, depthMat);
        // bool hasMaterial = matCollection.activateForPass(RenderingPass::DEPTH_PASS);

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

            //******************************************************************
            // SHADOW PASS
            currentRenderingPass = RenderingPass::DEPTH_PASS;

            lightManager.setLightMatrixForBatch(0);

            activateMaterials = false;

            shadowMapGeneratorProgram.useProgram();

            // glBindFramebuffer(GL_FRAMEBUFFER, fbo);
            fbo.bindToTarget(FramebufferTarget::FRAMEBUFFER);

            glDisable(GL_BLEND);
            glViewport(0, 0, 1024, 1024);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            cube.render();
            secondEntity.render();

            //******************************************************************
            // regular pass
            currentRenderingPass = RenderingPass::REGULAR_PASS;

            activateMaterials = true;
            cameraBuffer.updateData(glm::value_ptr(cameraController.getCamera().calculateWorldToClipMatrix()),
                                    sizeof(glm::mat4), 0);
            cameraBuffer.updateData(glm::value_ptr(cameraController.getCamera().getWorldToCameraMatrix()), sizeof(glm::mat4),
                                    sizeof(glm::mat4));
            cameraBuffer.updateData(glm::value_ptr(cameraController.getCamera().getPosition()), sizeof(glm::vec4),
                                    sizeof(glm::mat4) * 2);
            GLFramebufferObject::clearFramebufferAtTarget(FramebufferTarget::FRAMEBUFFER);
            glDisable(GL_BLEND);  // first pass disable blend because when you render, if a triangle is rendered in the
                                  // background, and then something renders on top of it, the background and
            // the triangle on top will be blended together.
            program1.useProgram();

            glViewport(0, 0, 800, 600);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            lightManager.sendBatchToShader(0);

            // bind depth map
            depthTexture.bindToTextureUnit(2);

            cube.render();
            secondEntity.render();

            // additoinal passes, enable blend
            glEnable(GL_BLEND);
            for (int i = 1; i < lightManager.getBatchCount(); ++i) {
                lightManager.sendBatchToShader(i);
                cube.render();
            }

            //******************************************************************
            // random blit pass

            // glDisable(GL_BLEND);

            // // view depth buffer
            // GLFramebufferObject::clearFramebufferAtTarget(FramebufferTarget::FRAMEBUFFER);
            // depthTexture.bindToTextureUnit(0);
            // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // glBindVertexArray(vao);
            // texturedRect.useProgram();
            // glDrawArrays(GL_TRIANGLES, 0, 6);

            window.display();
        }
    }
    return 0;
}