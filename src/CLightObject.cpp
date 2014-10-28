#include "CLightObject.h"


CLightObject::CLightObject() : position(glm::vec3(0.0, 0.0, 0.0)), direction(glm::vec3(0.5f, 2, 2)), up(glm::vec3(0, 1, 0))
{
	//computeMatrixes();
}

CLightObject::CLightObject(glm::vec3 pos, glm::vec3 dir) : up(glm::vec3(0, 1, 0))
{
	this->position = pos;
	this->direction = dir;
	//computeMatrixes();
}


CLightObject::~CLightObject()
{
}


void CLightObject::computeMatrixes() {
	//float aspec = (float)(SHADOWMAPSIZE) /(float)(SHADOWMAPSIZE);
	this->ProjectionMatrix = glm::perspective<float>(90.0f, 1.0, 0.1f, 1000.0f);
	// Camera matrix
	this->ViewMatrix = glm::lookAt(
		this->position,
		this->position - this->direction,
		this->up
		);
}

glm::vec3 CLightObject::getPosition() {
	return this->position;
}

void CLightObject::setPosition(glm::vec3 p) {
	this->position = p;
}

glm::vec3 CLightObject::getDirection() {
	return this->direction;
}

void CLightObject::setDirection(glm::vec3 dir) {
	this->direction = dir;
}


glm::mat4 CLightObject::getProjMatrix() {
	return this->ProjectionMatrix;
}
glm::mat4 CLightObject::getViewMatrix() {
	return this->ViewMatrix;
}

void CLightObject::visualize() {

}