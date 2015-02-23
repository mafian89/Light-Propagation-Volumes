#include "CBoundingBox.h"

CBoundingBox::CBoundingBox() : min(glm::vec3(0.0)), max(glm::vec3(0.0)) {

}

CBoundingBox::CBoundingBox(glm::vec3 min, glm::vec3 max) {
	this->min = min;
	this->max = max;
}

CBoundingBox::~CBoundingBox() {

}

glm::vec3 CBoundingBox::getMax() {
	return this->max;
}

glm::vec3 CBoundingBox::getMin() {
	return this->min;
}