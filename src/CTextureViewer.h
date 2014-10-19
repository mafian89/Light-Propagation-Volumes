#ifndef CTEXTUREVIEWER_H
#define CTEXTUREVIEWER_H

#include "common.h"

/*
* Simple class, which will render texture on screen 
*/
class CTextureViewer
{
private:
	GLuint texture;
	//VBO - don't need EBO, i'll use glDrawArrays()
	GLuint VBO;
	//VAO - needed for glDrawArrays()
	GLuint VAO;
	GLSLShader * s;
	//Default shaders
	std::string vs;
	std::string fs;
	void setUpShaders();
public:
	CTextureViewer();
	CTextureViewer(GLuint tex, std::string vs, std::string fs);
	void draw();
	//Setters
	void setTexture(GLuint tex);
	//Getters
	GLuint getTexture();
	~CTextureViewer();
};

#endif

