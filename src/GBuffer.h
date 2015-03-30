#ifndef GBUFFER_H
#define GBUFFER_H

#include "common.h"

class CFboManager;
class CTextureManager;

class GBuffer {
private:
	int width, height;
	CFboManager * fboMan;
	CTextureManager & texMan;

public:
	GBuffer(CTextureManager & tMgr, int w, int h);
	~GBuffer();
	void bindToRender();
	void unbind();
};



#endif