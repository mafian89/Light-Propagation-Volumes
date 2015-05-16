/* 
 * File:   animationCamera.h
 * Desc:	Animacni kamera, ktera se pouziva pro intepolaci
 *
 */

#ifndef ANIMATIONCAMERA_H
#define	ANIMATIONCAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


class animationCamera {
public:
	/*
	Konstruktor, destruktor
	*/
    animationCamera();
    ~animationCamera();
    
	/*Operace s kamerou*/
    void setAnimationCameraAtPosition(glm::vec3 pos, glm::vec3 dir, glm::vec3 up);
    void setAnimationCameraProperties(float n, float f, float fov, float aspect);
    void cameraLook();
    void init();
    
    /*Nastaveni vlastnosti kamery. V radianech*/
    void setAnimationCameraVerticalAngle(float angle);
    void setAnimationCameraHorizontalAngle(float angle);
    void setAnimationCameraPosition(glm::vec3 pos);
    void setAnimationCameraRight(glm::vec3 right);
    void setAnimationCameraFov(float fieldofview);
    /*
    void setAnimationCameraViewMatrix(glm::mat4);
    void setAnimationCameraProjectionMatrix(glm::mat4);
    */
    
    /*Gettery*/
    glm::vec3 getAnimationCameraPosition();
    glm::vec3 getAnimationCameraDirection();
    glm::vec3 getAnimationCameraUp();
    glm::vec3 getAnimationCameraRight();
    glm::mat4 getAnimationCameraViewMatrix();
    glm::mat4 getAnimationCameraProjectionMatrix();
    float getAnimationCameraHorizontalAngle();
    float getAnimationCameraVerticalAngle();
    
    float nearPlane, farPlane, fov, aspectRatio, verticalAngle, horizontalAngle,
          speed;
    
private:
    glm::vec3 _position, _direction, _up,_right;
    glm::mat4 viewMatrix, projMatrix;
};

#endif	/* ANIMATIONCAMERA_H */

