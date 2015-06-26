/*
* File:   DebugDrawer.h
* Desc:		Jednotucha trida pro vykreslovani v ramci ladeni
*
*/
#ifndef DEBUGDRAWER_H
#define DEBUGDRAWER_H

#include "common.h"
#include "GLSLShader/GLSLShader.h"

class DebugDrawer {
public:
	DebugDrawer();
	DebugDrawer(GLenum mode, const std::vector<glm::vec3> *p, const std::vector<glm::vec2> *uv, const std::vector<float> *i, glm::vec3 color);
	~DebugDrawer();
	void setVPMatrix(glm::mat4 vp);
	void setDrawColor(glm::vec3 c) {
		color = c;
	}
	void draw();
	void updateVBO(const std::vector<glm::vec3> *p);
private:
	GLenum mode;
	glm::vec3 color;
	std::vector<glm::vec3> p;
	std::vector<glm::vec2> uv;
	std::vector<float> i;
	GLSLShader * s;
	GLuint VAO, VBO, EBO;
	bool useUv, useIndicies;
	std::string fs, vs;
	glm::mat4 vp;
	void setUpShaders();
};

#endif