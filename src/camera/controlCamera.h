/* 
 * File:   control.h
 * Author: http://www.opengl-tutorial.org/beginners-tutorials/tutorial-6-keyboard-and-mouse/
 * Taken from the website written above
 *
 * Created on 1. listopad 2012, 18:42
 */

#ifndef CONTROLCAMERA_H
#define	CONTROLCAMERA_H

#include "../common.h"

class CControlCamera {
	private:
		glm::mat4 ViewMatrix;
		glm::mat4 ProjectionMatrix;
		glm::vec3 position;
		glm::vec3 initial_position;
		glm::vec3 direction;
		glm::vec3 right;
		glm::vec3 up;
		float horizontalAngle;
		float initial_horizontalAngle;
		float verticalAngle;
		float initial_verticalAngle;
		float fov;
		float aspec;
		unsigned width;
		unsigned height;
		float near_clip_plane;
		float far_clip_plane;
		int _x,_y;
		Uint8 *keys;
		SDL_Window * window;
	public:
		CControlCamera();
		~CControlCamera();
		
		//--Gettery
		glm::mat4 getViewMatrix();
		glm::mat4 getProjectionMatrix();
		glm::vec3 getPosition();
		glm::vec3 getInitialPosition();
		glm::vec3 getDirection();
		glm::vec3 getRight();
		glm::vec3 getUp();
		float getHorizontalAngle();
		float getInitialHorizontalAngle();
		float getVerticalAngle();
		float getInitialVerticalAngle();
		float mouseSpeed;
		bool moved;

		//--Settery
		void setHorizontalAngle(float);
		void setVerticalAngle(float);
		void setPosition(glm::vec3);
		void setDirection(glm::vec3);
		void setUp(glm::vec3);
		void setWindow(SDL_Window *);

		void initControlCamera(glm::vec3, SDL_Window *, float, float, unsigned, unsigned, float, float);
		void computeMatricesFromInputs();
};

#endif	/* CONTROLCAMERA_H */

