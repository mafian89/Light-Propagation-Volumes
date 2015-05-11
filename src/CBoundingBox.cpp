#include "CBoundingBox.h"
#include "Grid.h"

CBoundingBox::CBoundingBox() : min(glm::vec3(0.0)), max(glm::vec3(0.0)) {

}

CBoundingBox::CBoundingBox(glm::vec3 min, glm::vec3 max) : min(min), max(max) {
	calculatePointDimensions();
}

CBoundingBox::~CBoundingBox() {

}

glm::vec3 CBoundingBox::getMax() {
	return this->max;
}

glm::vec3 CBoundingBox::getMin() {
	return this->min;
}

std::vector<glm::vec3> CBoundingBox::getPoints() {
	return this->points;
}

std::vector<glm::vec3> CBoundingBox::getDebugDrawPoints() {
	return this->debugDrawPoints;
}

float CBoundingBox::getCellSize() {
	return this->cellSize;
}

Grid CBoundingBox::getGrid() {
	return Grid(max, min, 1.0f,0);
};

void CBoundingBox::calculatePointDimensions() {
	//std::cout << max.x - min.x << ", " << max.y - min.y << ", " << max.z - min.z << std::endl;

	//volSize = max - min;
	//float maxLength = std::max(volSize.x, std::max(volSize.y, volSize.z));
	//cellSize = maxLength / MAX_GRID_SIZE;

	//std::cout << "Max: " << max.x << "," << max.y << "," << max.z << std::endl;
	//std::cout << "Min: " << min.x << "," << min.y << "," << min.z << std::endl;
	//std::cout << "Cellsize: " << cellSize << std::endl;

	//width = int(volSize.x / cellSize + 0.5f);
	//height = int(volSize.y / cellSize + 0.5f);
	//depth = int(volSize.z / cellSize + 0.5f);

	//width = MAX_GRID_SIZE;
	//height = MAX_GRID_SIZE;
	//depth = MAX_GRID_SIZE;

	/*

	--FRONT--
	 (1)        (2)
	x_min,     x_max
	y_max,     y_max
	z_max      z_max
		O------O
		|      |
		|      |
		O------O
	x_min,     x_max
	y_min,     y_min
	z_max      z_max
	 (3)        (4)


	*/

	glm::vec3 p1 = glm::vec3(min.x,max.y,max.z);
	glm::vec3 p2 = glm::vec3(max.x, max.y, max.z);
	glm::vec3 p3 = glm::vec3(min.x, min.y, max.z);
	glm::vec3 p4 = glm::vec3(max.x, min.y, max.z);

	/*

	--BACK--
	(5)        (6)
	x_min,     x_max
	y_max,     y_max
	z_min      z_min
		O------O
		|      |
		|      |
		O------O
	x_min,     x_max
	y_min,     y_min
	z_min      z_min
	(7)        (8)


	*/

	glm::vec3 p5 = glm::vec3(min.x, max.y, min.z);
	glm::vec3 p6 = glm::vec3(max.x, max.y, min.z);
	glm::vec3 p7 = glm::vec3(min.x, min.y, min.z);
	glm::vec3 p8 = glm::vec3(max.x, min.y, min.z);


	points.push_back(p1);
	points.push_back(p2);
	points.push_back(p3);
	points.push_back(p4);

	points.push_back(p5);
	points.push_back(p6);
	points.push_back(p7);
	points.push_back(p8);

	debugDrawPoints.push_back(p1);
	debugDrawPoints.push_back(p2);
	debugDrawPoints.push_back(p4);
	debugDrawPoints.push_back(p3);
	debugDrawPoints.push_back(p1);
	debugDrawPoints.push_back(p5);
	debugDrawPoints.push_back(p7);
	debugDrawPoints.push_back(p3);
	debugDrawPoints.push_back(p4);
	debugDrawPoints.push_back(p8);
	debugDrawPoints.push_back(p7);
	debugDrawPoints.push_back(p5);
	debugDrawPoints.push_back(p6);
	debugDrawPoints.push_back(p2);
	debugDrawPoints.push_back(p6);
	debugDrawPoints.push_back(p8);
}

//void CBoundingBox::draw() {
//	std::cout << "Draw called" << std::endl;
//}

int CBoundingBox::getW() {
	return this->width;
}

int CBoundingBox::getH() {
	return this->height;
}
int CBoundingBox::getD() {
	return this->depth;
}

glm::vec3 CBoundingBox::getDimensions() {
	return glm::vec3(width,height,depth);
}