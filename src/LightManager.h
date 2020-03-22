#pragma once

#include <vector>
#include "Lights.h"
#include "./Buffer.h"
#include "defines.frag"

//TODO move to own folder with lights
//TODO write tests, disable copying?
class LightManager {

public:

	LightManager();

	void createDirectionalLight(const glm::vec4 &direction, const glm::vec4 &intensity);
	void createPointLight(const glm::vec4 &position, const glm::vec4 &intensity, const float &range);
	
	//connect the light buffers to the uniform buffer object in the shader so that data from the buffers
	//are received by the shaders
	void connectLightDataToShader();

	//reorganize the ligts so that the nearest lights are at the front of the array
	//this way when a batch of lights (the first X elements in the array) are sent to the shader, these lights are the closest
	void organizeImportantLights(glm::vec3 objectPos);

	//get the number of batches that exist
	//that is total number of lights / number of lights per batch
	int getBatchCount();

	//batch 0 = first X lights that are closest to the current object
	//batch 1 = next X lights, and so on
	//returns true if the batch was selected (there are enough lights to reach that many batches),
	//or returns false if the batch doesn't exist
	void sendBatchToShader(int batchId);

	const std::vector<DirectionalLight> &getDirectionalLights();
	const std::vector<PointLight> &getPointLights();


	static constexpr unsigned DIRECTIONAL_LIGHTS_PER_BATCH = MAX_DIRECTIONAL_LIGHTS;
	static constexpr unsigned POINT_LIGHTS_PER_BATCH = MAX_POINT_LIGHTS;

private:

	template <typename T>
	void sendLightBatchToShader(int batchId, const std::vector<T> &lightsCollection, Buffer &lightBuffer, const int maxLightInBatch, const T &dummyLight);


	std::vector<DirectionalLight> directionalLights;
	std::vector<PointLight> pointLights;

	//dummy lights to send to shader when there aren't enough lights to fill a batch
	static const DirectionalLight DUMMY_DIRECTIONAL_LIGHT;
	static const PointLight DUMMY_POINT_LIGHT;

	Buffer directionalLightBuffer;
	Buffer pointLightBuffer;
};