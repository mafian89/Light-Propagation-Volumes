#ifndef CBOUNDINGBOX_H
#define CBOUNDINGBOX_H

#include "common.h"

class IDrawable;

class CBoundingBox : public IDrawable {
private:
	std::vector<glm::vec3> points;
	std::vector<glm::vec3> debugDrawPoints;
	glm::vec3 min;
	glm::vec3 max;
	glm::vec3 volSize;
	float width, height, depth, cellSize;
	void calculatePointDimensions();
public:
	CBoundingBox();
	CBoundingBox(glm::vec3, glm::vec3);
	~CBoundingBox();
	glm::vec3 getMax();
	glm::vec3 getMin();
	std::vector<glm::vec3> getPoints();
	std::vector<glm::vec3> getDebugDrawPoints();
	virtual void draw();
};

#endif