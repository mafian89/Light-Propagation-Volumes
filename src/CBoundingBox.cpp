#include "CBoundingBox.h"

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

void CBoundingBox::calculatePointDimensions() {
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

void CBoundingBox::draw() {
	std::cout << "Draw called" << std::endl;
}