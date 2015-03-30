#include "GBuffer.h"

GBuffer::GBuffer(CTextureManager & tMgr, int w, int h) : texMan(tMgr), width(w), height(h)
{
	texMan.createTexture("g_Color", "", width, height, GL_NEAREST, GL_RGBA16F, GL_RGBA, false);
	texMan.createTexture("g_Normal", "", width, height, GL_NEAREST, GL_RGBA16F, GL_RGBA, false);
	texMan.createTexture("g_Depth", "", width, height, GL_LINEAR, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, true);

	fboMan = new CFboManager();
	fboMan->initFbo();
	fboMan->bindToFbo(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texMan["g_Color"]);
	fboMan->bindToFbo(GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, texMan["g_Normal"]);
	fboMan->bindToFbo(GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texMan["g_Depth"]);
	fboMan->setDrawBuffers();
	if (!fboMan->checkFboStatus()){
		return;
	}
}

void GBuffer::bindToRender() {
	glBindFramebuffer(GL_FRAMEBUFFER, fboMan->getFboId());
}

void GBuffer::unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GBuffer::~GBuffer()
{
	delete fboMan;
}