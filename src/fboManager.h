#ifndef FBOMANAGER_H
#define FBOMANAGER_H

#include "common.h"

class CFboManager {
public:
	CFboManager();
	~CFboManager();
	bool checkFboStatus();
	void genRenderDepthBuffer(unsigned w, unsigned h);
	void bindRenderDepthBuffer();
	void bindToFbo(GLenum type, GLenum texture, GLuint textureId);
	void bindLayeredTextureToFbo(GLenum type, GLenum texture, GLuint textureId, GLint layer);
	void initFbo();
	void setDrawBuffers();
	GLuint getFboId();
private:
	GLuint _fboId;
	GLuint _renderDepthBufferId;
	bool useRenderDepthBuffer;
	unsigned attachmentCount;
	GLenum mrt[6];
};

#endif