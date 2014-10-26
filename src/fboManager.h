#ifndef FBOMANAGER_H
#define FBOMANAGER_H

#include "common.h"

class CFboManager {
public:
	CFboManager();
	~CFboManager();
	bool checkFboStatus();
	void genRenderBuffer(unsigned w, unsigned h);
	void bindRenderBuffer();
	void bindToFbo(GLenum type, GLenum texture, GLuint textureId);
	void initFbo();
	void setDrawBuffers();
	GLuint getFboId();
private:
	GLuint _fboId;
	GLuint _renderBufferId;
	bool useRenderBuffer;
	unsigned attachmentCount;
	GLenum mrt[6];
};

#endif