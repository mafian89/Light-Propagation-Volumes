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

	//Model matrix
	glm::mat4 model = glm::translate(pos);
	glm::vec4 newCenter = model * glm::vec4(center, 1.0);

	//glm::vec3 centerToDir = pos  - center;
	//glm::vec3 newCenter = center + centerToDir; //translate
	//Snap to multiples of grid size
	//....
	//New volume's center
	glm::vec3 newMin = glm::vec3(newCenter.x, newCenter.y, newCenter.z) + centerToMin;

	//glm::vec3 tmpCenter = (max - min) / glm::vec3(2.0);
	//glm::vec3 tmpCenterToMin = min - tmpCenter;

	//std::cout <<"++++++++ " << "Level: " << level <<" +++++++\n";
	//std::cout << "pos: " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
	//std::cout << "center: " << center.x << ", " << center.y << ", " << center.z << std::endl;
	//std::cout << "newCenter: " << newCenter.x << ", " << newCenter.y << ", " << newCenter.z << std::endl;
	//std::cout << "min: " << min.x << ", " << min.y << ", " << min.z << std::endl;
	//std::cout << "newMin: " << newMin.x << ", " << newMin.y << ", " << newMin.z << std::endl;
	//std::cout << "centerToMin: " << centerToMin.x << ", " << centerToMin.y << ", " << centerToMin.z << std::endl;

	min = newMin;
	m = model;
	//center = glm::vec3(newCenter);

	//minToDir *= glm::vec3(cellSize); //snap??
	//min.x = min.x + glm::vec3(0.1).x*minToDir.x;
}