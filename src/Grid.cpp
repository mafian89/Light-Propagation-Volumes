#include "Grid.h"

Grid::Grid(const Grid & old, float scale, unsigned int level) {
	this->max = old.origMax;
	this->min = old.origMin;
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

	/*int halfGridSize = MAX_GRID_SIZE / 2;
	glm::vec3 frontFaceCenter = glm::vec3(min.x + halfGridSize * cellSize, min.y + halfGridSize * cellSize, min.z);
	glm::vec3 tmpCenterToMin = min - frontFaceCenter;

	glm::vec3 faceCenterToPos = pos - frontFaceCenter;
	glm::vec3 newFaceCenter = frontFaceCenter + faceCenterToPos;
	glm::vec3 newMin = newFaceCenter + tmpCenterToMin;*/

		glm::vec3 centerToDir = pos * glm::vec3(0.8) * dir - center;
		glm::vec3 newCenter = center + centerToDir; //translate
		glm::vec3 newMin = newCenter + centerToMin;

		//glm::vec3 tmpCenter = (max - min) / glm::vec3(2.0);
		//glm::vec3 tmpCenterToMin = min - tmpCenter;



		//std::cout << origMin.x << ", " << origMin.y << ", " << origMin.z << std::endl;
		//std::cout << newMin.x << ", " << newMin.y << ", " << newMin.z << std::endl;
		//std::cout << centerToMin.x << ", " << centerToMin.y << ", " << centerToMin.z << std::endl;

		min = newMin;


	//minToDir *= glm::vec3(cellSize); //snap??
	//min.x = min.x + glm::vec3(0.1).x*minToDir.x;
}