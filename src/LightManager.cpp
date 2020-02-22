#include "headers/LightManager.h"
#include "headers/ShaderProgram.h"

using std::vector;

const DirectionalLight LightManager::DUMMY_DIRECTIONAL_LIGHT = DirectionalLight{
	{1, 0, 0, 0},
	{0, 0, 0, 0}
}; 

const PointLight LightManager::DUMMY_POINT_LIGHT = PointLight{
	{0, 0, 0, 0},
	{0, 0, 0, 0},
	0,
	{0,0,0}
};

LightManager::LightManager() {
	pointLightBuffer.create(Buffer::BindingTarget::UniformBuffer, nullptr, sizeof(PointLight) * POINT_LIGHTS_PER_BATCH, Buffer::UsageType::StreamDraw);
	directionalLightBuffer.create(Buffer::BindingTarget::UniformBuffer, nullptr, sizeof(DirectionalLight) * DIRECTIONAL_LIGHTS_PER_BATCH, Buffer::UsageType::StreamDraw);
}

void LightManager::createDirectionalLight(const glm::vec4 &direction, const glm::vec4 &intensity) {

	directionalLights.push_back({direction, intensity});
}

void LightManager::createPointLight(const glm::vec4 &position, const glm::vec4 &intensity, const float &range) {

	pointLights.push_back({position, intensity, range});
}

void LightManager::connectLightDataToShader() {
	directionalLightBuffer.bindToTargetBindingPoint(ShaderProgram::DIRECTIONAL_LIGHT_UNIFORM_BLOCK_BINDING_POINT);
	pointLightBuffer.bindToTargetBindingPoint(ShaderProgram::POINT_LIGHT_UNIFORM_BLOCK_BINDING_POINT);
}

int LightManager::getBatchCount() {
	int direcitonalLightBatches = ceil((float)directionalLights.size() / DIRECTIONAL_LIGHTS_PER_BATCH);
	int pointLightBatches = ceil( (float)pointLights.size() / POINT_LIGHTS_PER_BATCH );

	return glm::max(direcitonalLightBatches, pointLightBatches);
}

bool LightManager::sendBatchToShader(int batchId) {
	//not enough batches, indicate failure
	if(batchId >= getBatchCount()) {
			return false;
	}

	//index into arrays where the current batch would start, if there are enough lights
	// int directionalLightBatchIndex = batchId * DIRECTIONAL_LIGHTS_PER_BATCH;
	// int pointLightBatchIndex = batchId * POINT_LIGHTS_PER_BATCH;

	// vector<DirectionalLight> directionalLightsToSend;

	// //get all the direcitonal lights that actually exist, and add them to the batch
	// directionalLightsToSend.insert(directionalLightsToSend.end(),
	// 		directionalLights.begin() + directionalLightBatchIndex, directionalLights.begin() + directionalLightBatchIndex + DIRECTIONAL_LIGHTS_PER_BATCH);

	
	// //determine if you need to use dummy lights for directional batch
	// //if you don't have enough lights to fill current batch, you will need dummy lights
	// int numDummyDirectional = glm::clamp<int>(directionalLightBatchIndex + DIRECTIONAL_LIGHTS_PER_BATCH - directionalLights.size(), DIRECTIONAL_LIGHTS_PER_BATCH, 0);

	// //add dummy directional lights
	// directionalLightsToSend.insert(directionalLightsToSend.end(), numDummyDirectional, DUMMY_DIRECTIONAL_LIGHT);

	// //put into light buffer which is connected to the shader uniform block
	// directionalLightBuffer.updateData(directionalLightsToSend.data(), sizeof(DirectionalLight) * directionalLightsToSend.size(), 0);
	// directionalLightBuffer.bindToTargetBindingPoint(ShaderProgram::DIRECTIONAL_LIGHT_UNIFORM_BLOCK_BINDING_POINT);

	// vector<PointLight> pointLightsToSend;

	// pointLightsToSend.insert(pointLightsToSend.end(),
	// 	pointLights.begin() + pointLightBatchIndex, pointLights.begin() + pointLightBatchIndex + POINT_LIGHTS_PER_BATCH);

	
	// int numDummyPoints = glm::clamp<int>(pointLightBatchIndex + POINT_LIGHTS_PER_BATCH - pointLights.size(), POINT_LIGHTS_PER_BATCH, 0);

	// pointLightsToSend.insert(pointLightsToSend.end(), numDummyPoints, DUMMY_POINT_LIGHT);

	// pointLightBuffer.updateData(pointLightsToSend.data(), sizeof(PointLight) * pointLightsToSend.size(), 0);
	// pointLightBuffer.bindToTargetBindingPoint(ShaderProgram::POINT_LIGHT_UNIFORM_BLOCK_BINDING_POINT);
	sendLightBatchToShader(batchId, directionalLights, directionalLightBuffer, DIRECTIONAL_LIGHTS_PER_BATCH, DUMMY_DIRECTIONAL_LIGHT);
	sendLightBatchToShader(batchId, pointLights, pointLightBuffer, POINT_LIGHTS_PER_BATCH, DUMMY_POINT_LIGHT);

	return true;
	
}

template<typename T>
void LightManager::sendLightBatchToShader(int batchId, const std::vector<T> &lightsCollection, Buffer &lightBuffer, const int maxLightInBatch, const T &dummyLight) {
	int batchIndex = batchId * maxLightInBatch;

	vector<T> lightsToSend;

	//make sure begin and end iterator is valid to avoid segfault 
	auto rangeStart = lightsCollection.size() < batchIndex ? lightsCollection.begin() : lightsCollection.begin() + batchIndex ;

	auto rangeEnd = (lightsCollection.size() <= batchIndex + maxLightInBatch) 
		? lightsCollection.end() : lightsCollection.begin() + batchIndex + maxLightInBatch;

	//get all the direcitonal lights that actually exist, and add them to the batch
	lightsToSend.insert(lightsToSend.end(),
			rangeStart, rangeEnd);


	//determine if you need to use dummy lights for directional batch
	//if you don't have enough lights to fill current batch, you will need dummy lights
	int numDummyLights = glm::clamp<int>(batchIndex + maxLightInBatch - lightsCollection.size(), 0, maxLightInBatch);

	//add dummy directional lights
	lightsToSend.insert(lightsToSend.end(), numDummyLights, dummyLight);

	//put into light buffer which is connected to the shader uniform block
	lightBuffer.updateData(lightsToSend.data(), sizeof(T) * lightsToSend.size(), 0);
}

const vector<DirectionalLight> &LightManager::getDirectionalLights() {

	return directionalLights;
}

const vector<PointLight> &LightManager::getPointLights() {

	return pointLights;
}