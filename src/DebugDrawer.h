#ifndef DEBUGDRAWER_H
#define DEBUGDRAWER_H

#include "common.h"

class DebugDrawer {
public:
	DebugDrawer();
	DebugDrawer(GLenum mode, const std::vector<glm::vec3> *p, const std::vector<glm::vec2> *uv, const std::vector<float> *i);
	~DebugDrawer();
	void setVPMatrix(glm::mat4 vp);
	void draw();
private:
	GLenum mode;
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