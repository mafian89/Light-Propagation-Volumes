#include "textureManager.h"

CTextureManager::CTextureManager() : clearTextureExtension(true) {
}

CTextureManager::~CTextureManager() {
	textures.clear();
}

void CTextureManager::add(const string& texture) {
	GLuint tmpId;
	glGenTextures(1, &tmpId);
	textures[texture] = tmpId;
}

void CTextureManager::createTexture(const string& texture, const string filePath,unsigned w, unsigned h, GLuint filter, GLuint type, GLuint type_2, bool depth = false){
	SDL_Surface *surface;
	GLuint textureid;
	int mode;
	add(texture);
	glBindTexture(GL_TEXTURE_2D, textures[texture]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//No image data - for frameBuffer
	if(filePath.empty()) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
		glTexImage2D(GL_TEXTURE_2D, 0 ,type, w, h, 0, type_2, GL_FLOAT, 0);
		glTexImage2D(GL_TEXTURE_2D, 0 ,type, w, h, 0, type_2, GL_FLOAT, 0);
		if (depth) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
			//glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
		}
	} 
}

void CTextureManager::createRGBA16F3DTexture(const string& texture, glm::vec3 dim, GLuint filter, GLuint wrap) {
	GLuint tex;
	add(texture);
	glBindTexture(GL_TEXTURE_3D, textures[texture]);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, filter);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, wrap);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, wrap);
	//glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA16F, w, h, d, 0, GL_RGBA, GL_FLOAT, NULL);
	//glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA16F, w, h, d, 0, GL_RGBA, GL_FLOAT, &emptyData[0]);
	std::vector<GLfloat> emptyData(dim.x * dim.y * dim.z * sizeof(float), 0.0);
	glBindTexture(GL_TEXTURE_3D, textures[texture]);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA16F, dim.x, dim.y, dim.z, 0, GL_RGBA, GL_FLOAT, &emptyData[0]);
	glBindTexture(GL_TEXTURE_3D, 0);
	//clear3Dtexture(textures[texture], dim);
	//glBindTexture(GL_TEXTURE_3D, 0);
}

void CTextureManager::clear3Dtexture(GLuint texture) {
	if (clearTextureExtension) {
		GLfloat data[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		glClearTexImage(texture, 0, GL_RGBA, GL_FLOAT, &data[0]);
	}
	else {
		//MUCH SLOWER version, but should work on version lower than 4.4
		std::vector<GLfloat> emptyData(MAX_GRID_SIZE * MAX_GRID_SIZE * MAX_GRID_SIZE * sizeof(float), 0.0);
		glBindTexture(GL_TEXTURE_3D, texture);
		//glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA16F, MAX_GRID_SIZE, MAX_GRID_SIZE, MAX_GRID_SIZE, 0, GL_RGBA, GL_FLOAT, &emptyData[0]);
		//or
		glTexSubImage3D(GL_TEXTURE_3D, 0, 0,0,0, MAX_GRID_SIZE, MAX_GRID_SIZE, MAX_GRID_SIZE, GL_RGBA, GL_FLOAT, &emptyData[0]);
		glBindTexture(GL_TEXTURE_3D, 0);
	}
}

void CTextureManager::createRGBA3DTexture(const string& texture, glm::vec3 dim, GLuint filter, GLuint wrap) {
	GLuint tex;
	add(texture);
	glBindTexture(GL_TEXTURE_3D, textures[texture]);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, filter);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, wrap);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, wrap);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, dim.x, dim.y, dim.z, 0, GL_RGBA, GL_FLOAT, NULL);
	//glBindTexture(GL_TEXTURE_3D, 0);
}

GLuint CTextureManager::operator[] (const string& texture) {
	return textures[texture];
}