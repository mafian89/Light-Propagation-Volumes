#ifndef CBOUNDINGBOX_H
#define CBOUNDINGBOX_H

#include "common.h"

//class IDrawable;

class Grid;

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
	CBoundingBox(glm::vec3 min, glm::vec3 max);
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
	Grid getGrid();
	static std::vector<glm::vec3> calculatePointDimensions(glm::vec3 _min, glm::vec3 _max) {
		std::vector<glm::vec3> debugDrawPoints;
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

		glm::vec3 p1 = glm::vec3(_min.x, _max.y, _max.z);
		glm::vec3 p2 = glm::vec3(_max.x, _max.y, _max.z);
		glm::vec3 p3 = glm::vec3(_min.x, _min.y, _max.z);
		glm::vec3 p4 = glm::vec3(_max.x, _min.y, _max.z);

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

		glm::vec3 p5 = glm::vec3(_min.x, _max.y, _min.z);
		glm::vec3 p6 = glm::vec3(_max.x, _max.y, _min.z);
		glm::vec3 p7 = glm::vec3(_min.x, _min.y, _min.z);
		glm::vec3 p8 = glm::vec3(_max.x, _min.y, _min.z);


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

		return debugDrawPoints;
	}
	//virtual void draw();
};

#endif