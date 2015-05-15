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
	//Model matrix - not sure about that multiplication by 0.5
	//glm::mat4 model = glm::translate(pos );
	//glm::vec4 newCenter = model * glm::vec4(center, 1.0);

	//glm::vec3 centerToDir = pos  - center;
	//glm::vec3 newCenter = center + centerToDir; //translate
	//Snap to multiples of grid size
	//....
	/*glm::vec3 centerToNewCenter = glm::vec3(newCenter) - center;
	int snapX = static_cast<int> (centerToNewCenter.x / cellSize + 0.5);
	int snapY = static_cast<int> (centerToNewCenter.y / cellSize + 0.5);
	int snapZ = static_cast<int> (centerToNewCenter.z / cellSize + 0.5);
	//New volume's center
	//glm::vec3 newMin = glm::vec3(newCenter.x, newCenter.y, newCenter.z) + centerToMin;
	glm::vec3 newMin = glm::vec3(center.x + (snapX)*cellSize, center.y + (snapY)*cellSize, center.z + (snapZ)*cellSize) + centerToMin;
	//glm::vec3 newMax = glm::vec3(center.x + (snapX)*cellSize, center.y + (snapY)*cellSize, center.z + (snapZ)*cellSize) + centerToMax;
	
	glm::vec3 displacement = glm::vec3(0.8)*dir;// *glm::vec3(0.5); //80% od direction
	glm::mat4 modelDispace = glm::translate(displacement); //translate
	glm::vec4 displacedMin = modelDispace * glm::vec4(newMin, 1.0); //new position of minimum
	//calculate difference between old and displaced min and then snap it to multiples of grid size
	glm::vec3 minToDisplacedMin = glm::vec3(displacedMin) - newMin;
	int snapMinX = static_cast<int> (minToDisplacedMin.x / cellSize + 0.5);
	int snapMinY = static_cast<int> (minToDisplacedMin.y / cellSize + 0.5);
	int snapMinZ = static_cast<int> (minToDisplacedMin.z / cellSize + 0.5);
	//newMin = glm::vec3(displacedMin.x + snapMinX*cellSize, displacedMin.y + snapMinY*cellSize, displacedMin.z + snapMinZ*cellSize);*/

	/*if ((snapMinX > 0 || snapMinY > 0 || snapMinZ > 0)) {
		std::cout << "++++++++ " << "Level: " << level << " +++++++\n";
		std::cout << "newMin.x: " << newMin.x << " newMin.x + snapMinX*cellSize: " << newMin.x + snapMinX*cellSize << std::endl;
		std::cout << "newMin.y: " << newMin.y << " newMin.y + snapMinY*cellSize: " << newMin.y + snapMinY*cellSize << std::endl;
		std::cout << "newMin.z: " << newMin.z << " newMin.z + snapMinZ*cellSize: " << newMin.z + snapMinZ*cellSize << std::endl;
		std::cout << "Displacement: " << displacement.x << ", " << displacement.y << ", " << displacement.z << std::endl;
		std::cout << "snapMinX: " << snapMinX << " snapMinY: " << snapMinY << " snapMinZ " << snapMinZ << std::endl;
	}*/
	//if (min != newMin) {
	//	std::cout << "++++++++ " << "Level: " << level << " +++++++\n";
	//	std::cout << "pos: " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
	//	std::cout << "center: " << center.x << ", " << center.y << ", " << center.z << std::endl;
	//	std::cout << "newCenter: " << newCenter.x << ", " << newCenter.y << ", " << newCenter.z << std::endl;
	//	std::cout << "min: " << min.x << ", " << min.y << ", " << min.z << std::endl;
	//	std::cout << "newMin: " << newMin.x << ", " << newMin.y << ", " << newMin.z << std::endl;
	//	std::cout << "centerToMin: " << centerToMin.x << ", " << centerToMin.y << ", " << centerToMin.z << std::endl;
	//}

	//new center = camera pos
	glm::vec4 newCenter = glm::vec4(pos, 1.0);

	glm::vec3 centerToNewCenter = glm::vec3(newCenter) - center;
	int snapX = static_cast<int> (centerToNewCenter.x / cellSize + 0.5);
	int snapY = static_cast<int> (centerToNewCenter.y / cellSize + 0.5);
	int snapZ = static_cast<int> (centerToNewCenter.z / cellSize + 0.5);

	//glm::vec3 newMin = glm::vec3(newCenter.x, newCenter.y, newCenter.z) + centerToMin;
	glm::vec3 newMin = glm::vec3(center.x + (snapX)*cellSize, center.y + (snapY)*cellSize, center.z + (snapZ)*cellSize) + centerToMin;
	glm::vec3 newMax = glm::vec3(center.x + (snapX)*cellSize, center.y + (snapY)*cellSize, center.z + (snapZ)*cellSize) + centerToMax;

	glm::vec3 displacement = glm::vec3(0.8)*dir*centerToMax*glm::vec3(0.5);
	glm::vec3 displacedMin = newMin - displacement;
	glm::vec3 displacedMax = newMax + displacement;
	//std::cout << "Displacement: " << displacement.x << ", " << displacement.y << ", " << displacement.z << std::endl;

	//newMin -= newMin * glm::vec3(0.2); //Move back by 20%
	/*if (min != newMin) {
		std::cout << "++++++++ " << "Level: " << level << " +++++++\n";
		std::cout << "pos: " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
		std::cout << "center: " << center.x << ", " << center.y << ", " << center.z << std::endl;
		std::cout << "newCenter: " << newCenter.x << ", " << newCenter.y << ", " << newCenter.z << std::endl;
		std::cout << "min: " << min.x << ", " << min.y << ", " << min.z << std::endl;
		std::cout << "newMin: " << newMin.x << ", " << newMin.y << ", " << newMin.z << std::endl;
		std::cout << "centerToMin: " << centerToMin.x << ", " << centerToMin.y << ", " << centerToMin.z << std::endl;
	}*/
	//if ((snapX > 0 || snapY > 0 || snapZ > 0)) {
	//std::cout << "++++++++ " << "Level: " << level << " +++++++\n";
	//std::cout << "newMin.x: " << newMin.x << " newMin.x + snapX*cellSize: " << newMin.x + snapX*cellSize << std::endl;
	//std::cout << "newMin.y: " << newMin.y << " newMin.y + snapY*cellSize: " << newMin.y + snapY*cellSize << std::endl;
	//std::cout << "newMin.z: " << newMin.z << " newMin.z + snapZ*cellSize: " << newMin.z + snapZ*cellSize << std::endl;
	//std::cout << "snapX: " << snapX << " snapY: " << snapY << " snapZ " << snapZ << std::endl;
	//}

	min = displacedMin;
	max = displacedMax;
	//max *= scale;
	//m = model;
	//center = glm::vec3(newCenter);
}