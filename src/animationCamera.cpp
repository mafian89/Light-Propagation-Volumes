/* 
 * File:   animationCamera.cpp
 * Desc:	Animacni kamera, ktera se pouziva pro intepolaci
 */

#include "animationCamera.h"

animationCamera::animationCamera() {
}

animationCamera::~animationCamera() {
}

/*Inicializace*/
void animationCamera::init() {
    this->setAnimationCameraProperties((float)0.1,(float)1000.0,this->fov,this->aspectRatio);
    this->setAnimationCameraAtPosition(this->getAnimationCameraPosition(), this->getAnimationCameraDirection(), this->getAnimationCameraUp());
    this->cameraLook();
}

void animationCamera::cameraLook() {
   this->viewMatrix = glm::lookAt(this->getAnimationCameraPosition(), this->getAnimationCameraDirection()+this->getAnimationCameraPosition(), this->getAnimationCameraUp()); 
   this->projMatrix = glm::perspective(this->fov, this->aspectRatio, this->nearPlane, this->farPlane);
}

/*Settery*/
void animationCamera::setAnimationCameraHorizontalAngle(float angle) {
    this->horizontalAngle = angle;
}

void animationCamera::setAnimationCameraVerticalAngle(float angle) {
    this->verticalAngle = angle;
}

void animationCamera::setAnimationCameraRight(glm::vec3 right) {
    this->_right = right; 
}

void animationCamera::setAnimationCameraAtPosition(glm::vec3 pos, glm::vec3 dir, glm::vec3 up) {
    this->_position = pos;
    this->_direction = dir;
    this->_up = up; 
}
void animationCamera::setAnimationCameraProperties(float n, float f, float fieldofview, float aspect){
    this->nearPlane = n;
    this->farPlane = f;
    this->fov = fieldofview;
    this->aspectRatio = aspect;
}

void animationCamera::setAnimationCameraPosition(glm::vec3 pos) {
    this->_position = pos;
}

void animationCamera::setAnimationCameraFov(float fieldofview) {
    this->fov = fieldofview;
}

/*Getters*/
glm::vec3 animationCamera::getAnimationCameraPosition(){
    return this->_position;
}
glm::vec3 animationCamera::getAnimationCameraDirection(){
    return this->_direction;
}
glm::vec3 animationCamera::getAnimationCameraUp(){
    return this->_up;
}
glm::mat4 animationCamera::getAnimationCameraViewMatrix(){
    return this->viewMatrix;
}
glm::mat4 animationCamera::getAnimationCameraProjectionMatrix(){
    return this->projMatrix;
}

float animationCamera::getAnimationCameraHorizontalAngle() {
    return this->horizontalAngle;
}

float animationCamera::getAnimationCameraVerticalAngle() {
    return this->verticalAngle;
}

glm::vec3 animationCamera::getAnimationCameraRight() {
    return this->_right;
}