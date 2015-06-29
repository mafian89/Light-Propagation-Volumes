/*
* File:   Grid.h
* Desc:		Reprezetace mrizky
*
*/
#ifndef GRID_H
#define GRID_H

#include "common.h"

class Grid {
public:
	Grid() {};
	Grid(glm::vec3 max, glm::vec3 min, float scale, unsigned int level);
	Grid(const Grid & old, float scale, unsigned int level);
	~Grid();
	glm::vec3 getMin() { return min; };
	glm::vec3 getMax() { return max; };
	glm::ivec3 getDimensions() { return dimensions; };
	float getScale() { return scale; };
	float getCellSize() { return cellSize; };
	void translateGrid(glm::vec3 pos, glm::vec3 dir);
	glm::mat4 getModelMatrix() { return m; };
	glm::vec3 getCenter() { return center; }

private:
	void setUp() {
		center = glm::vec3(0);
		cellSize = MAX_CELL_SIZE * scale;
		min = center - glm::vec3(MAX_GRID_SIZE*0.5 * cellSize);
		max = center + glm::vec3(MAX_GRID_SIZE*0.5 * cellSize);
		centerToMin = min - center;
		centerToMax = max - center;

		std::cout << "centerToMin: " << centerToMin.x << "," << centerToMin.y << "," << centerToMin.z << std::endl;
		std::cout << "centerToMax: " << centerToMax.x << "," << centerToMax.y << "," << centerToMax.z << std::endl;
		
		dimensions.x = MAX_GRID_SIZE;
		dimensions.y = MAX_GRID_SIZE;
		dimensions.z = MAX_GRID_SIZE;

		std::cout << "Max: " << max.x << "," << max.y << "," << max.z << std::endl;
		std::cout << "Min: " << min.x << "," << min.y << "," << min.z << std::endl;
		std::cout << "center: " << center.x << "," << center.y << "," << center.z << std::endl;
		std::cout << "Cellsize: " << cellSize << std::endl;

	};
	glm::vec3 min, max , center, offset, centerToMin, centerToMax;
	glm::vec3 origMin, origMax;
	glm::ivec3 dimensions;
	float scale, cellSize;
	unsigned int level;
	glm::mat4 m;
};

#endif