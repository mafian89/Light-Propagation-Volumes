#ifndef CBOUNDINGBOX_H
#define CBOUNDINGBOX_H

#include "common.h"

class CBoundingBox {
private:
	glm::vec3 min;
	glm::vec3 max;
public:
	CBoundingBox();
	CBoundingBox(glm::vec3, glm::vec3);
	~CBoundingBox();
	glm::vec3 getMax();
	glm::vec3 getMin();
};

#endif