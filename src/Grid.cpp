#include "Grid.h"

Grid::Grid(const Grid & old, float scale) {
	this->max = old.max;
	this->min = old.min;
	this->scale = scale;
	setUp();
}

Grid::Grid(glm::vec3 max, glm::vec3 min, float scale) {
	this->max = max;
	this->min = min;
	this->scale = scale;
	setUp();
}

Grid::~Grid() {
}

void Grid::translateGrid(glm::vec3 dir) {


	glm::vec3 centerToDir = dir - center;
	glm::vec3 newCenter = center + centerToDir; //translate
	glm::vec3 newMin = newCenter + centerToMin;

	//std::cout << newMin.x << ", " << newMin.y << ", " << newMin.z << std::endl;
	//std::cout << centerToMin.x << ", " << centerToMin.y << ", " << centerToMin.z << std::endl;

	min.z = newMin.z;

	//minToDir *= glm::vec3(cellSize); //snap??
	//min.x = min.x + glm::vec3(0.1).x*minToDir.x;
}