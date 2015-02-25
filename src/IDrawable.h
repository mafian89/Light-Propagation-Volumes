#ifndef IDRAWABLE_H
#define IDRAWABLE_H

#include "common.h"

class IDrawable {
public:
	virtual void draw() = 0;
	virtual ~IDrawable() {};
};

#endif