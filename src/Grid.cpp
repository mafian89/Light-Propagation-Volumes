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
	//Model matrix
	glm::mat4 model = glm::translate(pos);
	glm::vec4 newCenter = model * glm::vec4(center, 1.0);

	//glm::vec3 centerToDir = pos  - center;
	//glm::vec3 newCenter = center + centerToDir; //translate
	//Snap to multiples of grid size
	//....
	glm::vec3 centerToNewCenter = glm::vec3(newCenter) - center;
	int snapX = static_cast<int> (centerToNewCenter.x / cellSize + 0.5);
	int snapY = static_cast<int> (centerToNewCenter.y / cellSize + 0.5);
	int snapZ = static_cast<int> (centerToNewCenter.z / cellSize + 0.5);
	//std::cout << "snapX: " << snapX << " snapY: " << snapY << " snapZ: " << snapZ << std::endl;
	//New volume's center
	//glm::vec3 newMin = glm::vec3(newCenter.x, newCenter.y, newCenter.z) + centerToMin;
	glm::vec3 newMin = glm::vec3(center.x + abs(snapX)*cellSize, center.y + abs(snapY)*cellSize, center.z + abs(snapZ)*cellSize) + centerToMin;

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
}