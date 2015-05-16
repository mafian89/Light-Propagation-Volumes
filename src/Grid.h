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
		/*max *= scale;
		min *= scale;
		glm::vec3 volSize = max - min;
		float maxLength = std::max(volSize.x, std::max(volSize.y, volSize.z));
		cellSize = maxLength / MAX_GRID_SIZE;*/

		center = glm::vec3(0);
		cellSize = 2.5 * scale;
		min = center - glm::vec3(MAX_GRID_SIZE*0.5 * cellSize);
		max = center + glm::vec3(MAX_GRID_SIZE*0.5 * cellSize);
		centerToMin = min - center;
		centerToMax = max - center;

		std::cout << "centerToMin: " << centerToMin.x << "," << centerToMin.y << "," << centerToMin.z << std::endl;
		std::cout << "centerToMax: " << centerToMax.x << "," << centerToMax.y << "," << centerToMax.z << std::endl;
		

		//dimensions.x = int(volSize.x / cellSize + 0.5f);
		//dimensions.y = int(volSize.y / cellSize + 0.5f);
		//dimensions.z = int(volSize.z / cellSize + 0.5f);

		dimensions.x = MAX_GRID_SIZE;
		dimensions.y = MAX_GRID_SIZE;
		dimensions.z = MAX_GRID_SIZE;

		//if (cellSize > 2.5)
		//	cellSize = 2.5;
		//
		//if (cellSize <= 2.5) {

		//	origMin = min;
		//	origMax = max;

		//	if(level == 0)
		//		cellSize *= 1.4;
		//	min.x -= (MAX_GRID_SIZE * cellSize - max.x) / 2.0;
		//	min.y -= (MAX_GRID_SIZE * cellSize - max.y) / 2.0;
		//	min.z -= (MAX_GRID_SIZE * cellSize - max.z) / 2.0;

		//	max.x += (MAX_GRID_SIZE * cellSize - max.x) / 2.0;
		//	max.y += (MAX_GRID_SIZE * cellSize - max.y) / 2.0;
		//	max.z += (MAX_GRID_SIZE * cellSize - max.z) / 2.0;

		//}

		/*volSize = max - min;
		//center = volSize / glm::vec3(2.0f);
		int halfSize = MAX_GRID_SIZE / 2;
		glm::vec3 frontFaceCenter = glm::vec3(min.x + halfSize * cellSize, min.y + halfSize * cellSize, min.z);
		center = frontFaceCenter;
		centerToMin = min - center;
		centerToMax = max - center;*/

		std::cout << "Max: " << max.x << "," << max.y << "," << max.z << std::endl;
		std::cout << "Min: " << min.x << "," << min.y << "," << min.z << std::endl;
		std::cout << "center: " << center.x << "," << center.y << "," << center.z << std::endl;
		std::cout << "Cellsize: " << cellSize << std::endl;

		//return ivec3((pos - v_min) / f_cellSize + 0.5 * normal);

		glm::vec3 test = glm::vec3((min - min) / cellSize) / (glm::vec3(dimensions)*glm::vec3(scale));
		std::cout << "Grid [min]: " << test.x << "," << test.y << "," << test.z <<  std::endl;
		//glm::vec3 newPoint = min + glm::vec3(10.0);
		//test = glm::ivec3((newPoint - min) / cellSize);
		//std::cout << "Grid [min-10]: " << test.x << "," << test.y << "," << test.z << " isBorder? " << isBorder(test) << std::endl;
		//std::cout << "Grid [min-10]: " << test.x << "," << test.y << "," << test.z << " isInside? " << isInside(test) << std::endl;
		//newPoint = max + glm::vec3(50.0);
		//test = glm::ivec3((newPoint - min) / cellSize);
		//std::cout << "Grid [max+50]: " << test.x << "," << test.y << "," << test.z << " isInside? " << isInside(test) << std::endl;

	};
	glm::vec3 min, max , center, offset, centerToMin, centerToMax;
	glm::vec3 origMin, origMax;
	glm::ivec3 dimensions;
	float scale, cellSize;
	unsigned int level;
	glm::mat4 m;

	bool isBorder(glm::ivec3 i) {
		if (i.x == 0 || i.x > int(dimensions.x))
			return true;
		if (i.y == 0 || i.y > int(dimensions.y))
			return true;
		if (i.z == 0 || i.z > int(dimensions.z))
			return true;
		return false;
	}

	/*bool isInside(glm::ivec3 i) {
		if (i.x < 0 || i.x > int(dimensions.x))
			return false;
		if (i.y < 0 || i.y > int(dimensions.y))
			return false;
		if (i.z < 0 || i.z > int(dimensions.z))
			return false;
		return true;
	}*/
};

#endif