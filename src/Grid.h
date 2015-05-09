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
	void translateGrid(glm::vec3 dir);
private:
	void setUp() {
		max *= scale;
		min *= scale;
		glm::vec3 volSize = max - min;
		center = volSize / glm::vec3(2.0f);
		centerToMin = min - center;
		float maxLength = std::max(volSize.x, std::max(volSize.y, volSize.z));
		cellSize = maxLength / MAX_GRID_SIZE;

		std::cout << "Max: " << max.x << "," << max.y << "," << max.z << std::endl;
		std::cout << "Min: " << min.x << "," << min.y << "," << min.z << std::endl;
		std::cout << "Cellsize: " << cellSize << std::endl;

		dimensions.x = int(volSize.x / cellSize + 0.5f);
		dimensions.y = int(volSize.y / cellSize + 0.5f);
		dimensions.z = int(volSize.z / cellSize + 0.5f);

		//return ivec3((pos - v_min) / f_cellSize + 0.5 * normal);
		/*glm::ivec3 test = glm::ivec3((min - min) / cellSize);
		std::cout << "Grid [min]: " << test.x << "," << test.y << "," << test.z << std::endl;
		glm::vec3 newPoint = min - glm::vec3(10.0);
		test = glm::ivec3((newPoint - min) / cellSize);
		std::cout << "Grid [min-10]: " << test.x << "," << test.y << "," << test.z << std::endl;
		newPoint = max + glm::vec3(10.0);
		test = glm::ivec3((newPoint - min) / cellSize);
		std::cout << "Grid [max+10]: " << test.x << "," << test.y << "," << test.z << std::endl;*/
	};
	glm::vec3 min,max,center,offset,centerToMin;
	glm::ivec3 dimensions;
	float scale, cellSize;
};

#endif