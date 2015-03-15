#include "fboManager.h"

CFboManager::CFboManager() {
	useRenderDepthBuffer = false;
	attachmentCount = 0;
	/*for(int i=0;i<5;i++) {
		mrt[i] = GL_COLOR_ATTACHMENT0+i;
	}*/
}

CFboManager::~CFboManager() {
	glDeleteFramebuffers(1, &_fboId);
	if(useRenderDepthBuffer) {
		glDeleteRenderbuffers(1, &_renderDepthBufferId);
	}
}

void CFboManager::initFbo() {
	glGenFramebuffers(1, &_fboId);
}

void CFboManager::genRenderDepthBuffer(unsigned w, unsigned h) {
	glGenRenderbuffers(1,&_renderDepthBufferId);
	glBindRenderbuffer(GL_RENDERBUFFER, _renderDepthBufferId);
	glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT,w,h);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void CFboManager::bindRenderDepthBuffer() {
	glBindFramebuffer(GL_FRAMEBUFFER,_fboId);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,_renderDepthBufferId);
	//glBindFramebuffer(GL_FRAMEBUFFER,0);
}

void CFboManager::setDrawBuffers() {
	glDrawBuffers(attachmentCount, mrt);
}

void CFboManager::bindToFbo(GLenum type, GLenum texture, GLuint textureId) {
	glBindFramebuffer(GL_FRAMEBUFFER,_fboId);
	glFramebufferTexture2D(GL_FRAMEBUFFER,type,texture,textureId,0);
	if(type != GL_DEPTH_ATTACHMENT) {
		mrt[attachmentCount] = type;
		attachmentCount += 1;
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	}
	//glBindFramebuffer(GL_FRAMEBUFFER,0);
}

void CFboManager::bind3DTextureToFbo(GLenum type, GLuint textureId) {
	glBindFramebuffer(GL_FRAMEBUFFER, _fboId);
	//glFramebufferTexture(GL_FRAMEBUFFER, type, textureId, 0);
	glFramebufferTexture3D(GL_FRAMEBUFFER, type, GL_TEXTURE_3D, textureId, 0, 0);
	if (type != GL_DEPTH_ATTACHMENT) {
		mrt[attachmentCount] = type;
		attachmentCount += 1;
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	}
	/*static const GLenum draw_buffers[] =
	{
	GL_COLOR_ATTACHMENT0
	};
	glDrawBuffers(1, draw_buffers);*/
}

GLuint CFboManager::getFboId() {
	return _fboId;
}

bool CFboManager::checkFboStatus() {
	// check FBO status
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	switch(status)
	{
	case GL_FRAMEBUFFER_COMPLETE_EXT:
		std::cout << "Framebuffer complete." << std::endl;
		return true;

	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
		std::cout << "[ERROR] Framebuffer incomplete: Attachment is NOT complete." << std::endl;
		return false;

	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
		std::cout << "[ERROR] Framebuffer incomplete: No image is attached to FBO." << std::endl;
		return false;

	case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
		std::cout << "[ERROR] Framebuffer incomplete: Attached images have different dimensions." << std::endl;
		return false;

	case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
		std::cout << "[ERROR] Framebuffer incomplete: Color attached images have different internal formats." << std::endl;
		return false;

	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
		std::cout << "[ERROR] Framebuffer incomplete: Draw buffer." << std::endl;
		return false;

	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
		std::cout << "[ERROR] Framebuffer incomplete: Read buffer." << std::endl;
		return false;

	case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
		std::cout << "[ERROR] Unsupported by FBO implementation." << std::endl;
		return false;

	default:
		std::cout << "[ERROR] Unknow error." << std::endl;
		return false;
	}
}