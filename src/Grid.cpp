#include "Grid.h"

Grid::Grid(const Grid & old, float scale) {
	this->max = old.max;
	this->min = old.min;
	this->scale = scale;
	setUp();
}

Grid::Grid(glm::vec3 max, glm::vec3 min, float scale) {
	this->max = max;
	this->min = min;
	this->scale = scale;
	setUp();
}

Grid::~Grid() {
}