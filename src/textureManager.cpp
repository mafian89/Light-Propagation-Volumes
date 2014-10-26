#include "textureManager.h"

CTextureManager::CTextureManager() {
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
	//else {
	//	surface = IMG_Load(filePath.c_str());
	//	if (!surface) {
	//		std::cout<<"Cannot load image!"<<std::endl;
	//		std::cout << filePath << std::endl;
	//		return;
	//	}
	//	if (surface->format->BytesPerPixel == 3) { // RGB 24bit
	//			mode = GL_RGB;
	//	} else if (surface->format->BytesPerPixel == 4) { // RGBA 32bit
	//		mode = GL_RGBA;
	//	} else {
	//		SDL_FreeSurface(surface);
	//	}
	//	glTexImage2D(GL_TEXTURE_2D, 0, mode, surface->w, surface->h, 0, mode, GL_UNSIGNED_BYTE, surface->pixels);
	//	SDL_FreeSurface(surface);
	//}
	//glBindTexture(GL_TEXTURE_2D,NULL);
}

GLuint CTextureManager::operator[] (const string& texture) {
	return textures[texture];
}