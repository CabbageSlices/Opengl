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

    // setup opengl context with version 4.6 core
    sf::Window window(sf::VideoMode(1024, 768), "OpenGL", sf::Style::Default,
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

        std::shared_ptr<MeshData> cubeMeshData = loadFromObj("cube.obj");
        std::shared_ptr<MeshRendererComponent> cubeMeshRenderer(new MeshRendererComponent(cubeMeshData));

        std::shared_ptr<MeshData> sphereMeshData = loadFromObj("smoothspherewhite.obj");
        std::shared_ptr<MeshRendererComponent> sphereMeshRenderer(new MeshRendererComponent(sphereMeshData));

        std::shared_ptr<MeshData> containerMeshData = loadFromObj("container.obj");
        std::shared_ptr<MeshRendererComponent> containerMeshRenderer(new MeshRendererComponent(containerMeshData));

        Entity container;
        container.addComponent(containerMeshRenderer);
        container.setRotation(0, 0, 90);

        vector<Entity*> entities;

        Entity cube;
        cube.addComponent(cubeMeshRenderer);
        cube.setPosition({-6, -3.6, 0});

        std::shared_ptr<MeshRendererComponent> secondMeshRendererComponent(new MeshRendererComponent(sphereMeshData));
        Entity secondEntity;
        secondEntity.addComponent(secondMeshRendererComponent);
        secondEntity.setPosition({-1.8, -2.5, -2});

        Entity entity3;
        entity3.addComponent(secondMeshRendererComponent);
        entity3.setPosition({-7, -3.6, -3});

        // entities.push_back(&cube);
        entities.push_back(&secondEntity);
        entities.push_back(&entity3);
        entities.push_back(&container);

        auto renderEntities = [&]() {
            for (auto& entity : entities) {
                entity->render();
            }
        };

        CameraController cameraController({0, 0, 5});

        LightManager lightManager;
        // lightManager.createDirectionalLight({0, 20, 0, 1}, {0, -1, 0, 0}, {1, 1, 1, 1});
        // lightManager.createDirectionalLight({-20, 0, 0, 1}, {-1, 0, 0, 0}, {0, 1, 0, 1});
        // lightManager.createDirectionalLight({0, 0, 1, 0}, {1, 1, 1, 1});
        // lightManager.createDirectionalLight({0, 0, -1, 0}, {1, 1, 1, 1});
        lightManager.createPointLight({-4.5, 1.5, -3, 1}, {0.8, 0.8, 0.8, 0.8}, 15);
        // lightManager.createPointLight({-2, 0, 4, 1}, {0.7, 0.7, 0.7, 0.7}, 15);

        lightManager.connectBuffersToShader();

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

        const GLsizei mapSize = 1024;

        GLTextureObject depthTexture(TextureType::TEXTURE_2D_ARRAY);
        depthTexture.create(1, TextureInternalStorageFormat::DEPTH_COMPONENT32F, mapSize, mapSize, MAX_DIRECTIONAL_LIGHTS);

        depthTexture.setParameterGroup(TextureParameterGroup::ZOOM, TextureFilteringModes::NEAREST);
        depthTexture.setParameterGroup(TextureParameterGroup::TEXTURE_WRAP, TextureWrapModes::CLAMP_TO_EDGE);

        GLTextureObject pointlightDepthTexture(TextureType::TEXTURE_CUBEMAP_ARRAY);

        /*must be number of layer faces, not layers. divisible by 6
         * https://www.khronos.org/opengl/wiki/Cubemap_Texture#Cubemap_array_textures*/
        pointlightDepthTexture.create(1, TextureInternalStorageFormat::DEPTH_COMPONENT32F, mapSize, mapSize,
                                      MAX_POINT_LIGHTS * 6);
        pointlightDepthTexture.setParameterGroup(TextureParameterGroup::ZOOM, TextureFilteringModes::NEAREST);
        pointlightDepthTexture.setParameterGroup(TextureParameterGroup::TEXTURE_WRAP, TextureWrapModes::CLAMP_TO_EDGE);

        GLFramebufferObject fbo;
        fbo.create();
        fbo.attachTexture(FramebufferNonColorAttachment::DEPTH_ATTACHMENT, depthTexture, 0);
        fbo.enableDrawingToAttachment(FramebufferColorAttachment::NONE);
        // fbo.attachTexture(FramebufferColorAttachment::COLOR_ATTACHMENT0, colourTexture, 0, true);

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

        Buffer shadowMapFlagsBuffer;
        bufferSize = sizeof(int);
        shadowMapFlagsBuffer.create(Buffer::BufferType::UniformBuffer, 0, bufferSize, Buffer::UsageType::StreamDraw);
        shadowMapFlagsBuffer.bindToTargetBindingPoint(UNIFORM_SHADOW_MAP_FLAGS_BINDING_POINT);

        // GETTING SHADOW ARTIFACTS when lightsPerBatch != limit set by shader
        // this is because the depth texture doesn't have data proplery written for one of the lights
        // ALSO ADDITIONAL PASSES WILL HAVE FUCKED UP DETPH BUFFER SINCE I"M NOT WRITING TO DEPTH BUFFER IN EACH PASS
        // lightManager.setDirectionalLightsPerBatch(1);
        glViewport(0, 0, 1024, 768);

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

            //******************************************************************
            // SHADOW PASS

            currentRenderingPass = RenderingPass::DEPTH_PASS;

            // send a light batch so they ahve access to light data as well in depth shaders
            lightManager.sendLightBatchToShader(0);
            lightManager.sendLightMatrixBatchToShader(0);

            activateMaterials = false;

            fbo.attachTexture(FramebufferNonColorAttachment::DEPTH_ATTACHMENT, depthTexture, 0);
            fbo.bindToTarget(FramebufferTarget::FRAMEBUFFER);

            glDisable(GL_BLEND);
            glViewport(0, 0, mapSize, mapSize);
            glClearColor(0, 0, 1, 1);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            int shadowMapLightType = 0;  // direcitonal light shadow pass
            shadowMapFlagsBuffer.updateData(&shadowMapLightType, sizeof(shadowMapLightType), 0);

            renderEntities();

            // redraw for point lights
            fbo.attachTexture(FramebufferNonColorAttachment::DEPTH_ATTACHMENT, pointlightDepthTexture, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            shadowMapLightType = 1;  // point light shadow pass
            shadowMapFlagsBuffer.updateData(&shadowMapLightType, sizeof(shadowMapLightType), 0);

            renderEntities();

            //******************************************************************
            // regular pass

            glCullFace(GL_BACK);
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
            glClearColor(0, 0, 0, 0);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            lightManager.sendLightBatchToShader(0);

            // bind depth map
            depthTexture.bindToTextureUnit(DIRECTIONAL_LIGHTS_SHADOWMAP_TEXTURE_UNIT);
            pointlightDepthTexture.bindToTextureUnit(POINT_LIGHTS_SHADOWMAP_TEXTURE_UNIT);

            renderEntities();

            // additoinal passes, enable blend
            glEnable(GL_BLEND);
            for (int i = 1; i < lightManager.getBatchCount(); ++i) {
                lightManager.sendLightBatchToShader(i);

                renderEntities();
            }

            //******************************************************************
            // random blit pass

            // glDisable(GL_BLEND);

            // // view depth buffer
            // GLFramebufferObject::clearFramebufferAtTarget(FramebufferTarget::FRAMEBUFFER);
            // pointlightDepthTexture.bindToTextureUnit(0);
            // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // glBindVertexArray(vao);
            // texturedRect.useProgram();
            // glDrawArrays(GL_TRIANGLES, 0, 6);

            window.display();
        }
    }
    return 0;
}