#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include "common.h"

using namespace std;

class CTextureManager {
public:
	CTextureManager();
	~CTextureManager();
	GLuint operator[](const string& texture);
	void add(const string& texture);
	void createTexture(const string& texture, const string filePath, unsigned w, unsigned h, GLuint filter, GLuint type, GLuint type_2, bool depth);
	void createRGBA16F3DTexture(const string& texture, unsigned w, unsigned h, unsigned d, GLuint filter, GLuint wrap);
	void createRGBA3DTexture(const string& texture, unsigned w, unsigned h, unsigned d, GLuint filter, GLuint wrap);
private:
	map<string,GLuint> textures;
};

#endif