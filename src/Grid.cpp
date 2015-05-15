#include "Grid.h"

Grid::Grid(const Grid & old, float scale, unsigned int level) {
	this->max = old.max;
	this->min = old.min;
	this->scale = scale;
	this->level = level;
	setUp();
}

Grid::Grid(glm::vec3 max, glm::vec3 min, float scale, unsigned int level) {
	this->max = max;
	this->min = min;
	this->scale = scale;
	this->level = level;
	setUp();
}

Grid::~Grid() {
}

void Grid::translateGrid(glm::vec3 pos, glm::vec3 dir) {

	//new center = camera pos
	glm::vec4 newCenter = glm::vec4(pos, 1.0);

	glm::vec3 centerToNewCenter = glm::vec3(newCenter) - center;
	int snapX = static_cast<int> (centerToNewCenter.x / cellSize + 0.5);
	int snapY = static_cast<int> (centerToNewCenter.y / cellSize + 0.5);
	int snapZ = static_cast<int> (centerToNewCenter.z / cellSize + 0.5);

	//glm::vec3 newMin = glm::vec3(newCenter.x, newCenter.y, newCenter.z) + centerToMin;
	int xOffset = center.x + (snapX)*cellSize;
	int yOffset = center.y + (snapY)*cellSize;
	int zOffset = center.z + (snapZ)*cellSize;
	glm::vec3 offset = glm::vec3(xOffset, yOffset, zOffset);
	glm::vec3 newMin = offset + centerToMin;
	glm::vec3 newMax = offset + centerToMax;
	//-------
	float halfDisplacement = 0.8 * MAX_GRID_SIZE * 0.5 * cellSize;
	glm::vec3 displacement = dir*glm::vec3(halfDisplacement);
	int snapXDisp = static_cast<int> (displacement.x / cellSize + 0.5);
	int snapYDisp = static_cast<int> (displacement.y / cellSize + 0.5);
	int snapZDisp = static_cast<int> (displacement.z / cellSize + 0.5);
	//glm::vec3 displacedMin = newMin + displacement;
	//glm::vec3 displacedMax = newMax + displacement;
	glm::vec3 offsetDisp = glm::vec3(snapXDisp * cellSize, snapYDisp * cellSize, snapZDisp * cellSize);
	glm::vec3 displacedMin = newMin + offsetDisp;
	glm::vec3 displacedMax = newMax + offsetDisp;

	min = displacedMin;
	max = displacedMax;
}