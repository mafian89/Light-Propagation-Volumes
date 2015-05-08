#ifndef GRID_H
#define GRID_H

#include "common.h"

class Grid {
public:
	Grid() {};
	Grid(glm::vec3 max, glm::vec3 min, float scale);
	Grid(const Grid & old, float scale);
	~Grid();
	glm::vec3 getMin() { return min; };
	glm::vec3 getMax() { return max; };
	glm::ivec3 getDimensions() { return dimensions; };
	float getScale() { return scale; };
	float getCellSize() { return cellSize; };
private:
	void setUp() {
		max *= scale;
		min *= scale;
		glm::vec3 volSize = max - min;
		float maxLength = std::max(volSize.x, std::max(volSize.y, volSize.z));
		cellSize = maxLength / MAX_GRID_SIZE;

		std::cout << "Max: " << max.x << "," << max.y << "," << max.z << std::endl;
		std::cout << "Min: " << min.x << "," << min.y << "," << min.z << std::endl;
		std::cout << "Cellsize: " << cellSize << std::endl;

		dimensions.x = int(volSize.x / cellSize + 0.5f);
		dimensions.y = int(volSize.y / cellSize + 0.5f);
		dimensions.z = int(volSize.z / cellSize + 0.5f);
	};
	glm::vec3 min,max;
	glm::ivec3 dimensions;
	float scale, cellSize;
};

#endif