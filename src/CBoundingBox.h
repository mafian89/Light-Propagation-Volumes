#ifndef CBOUNDINGBOX_H
#define CBOUNDINGBOX_H

#include "common.h"

class IDrawable;

class CBoundingBox  {
private:
	std::vector<glm::vec3> points;
	std::vector<glm::vec3> debugDrawPoints;
	glm::vec3 min;
	glm::vec3 max;
	glm::vec3 volSize;
	int width, height, depth;
	float cellSize;
	void calculatePointDimensions();
public:
	CBoundingBox();
	CBoundingBox(glm::vec3, glm::vec3);
	~CBoundingBox();
	glm::vec3 getMax();
	glm::vec3 getMin();
	std::vector<glm::vec3> getPoints();
	std::vector<glm::vec3> getDebugDrawPoints();
	float getCellSize();
	int getW();
	int getH();
	int getD();
	glm::vec3 getDimensions();
	//virtual void draw();
};

#endif