/* 
 * File:   cubic.h
 * Desc: Trida, pro kubicky spline
 */

#ifndef CUBIC_H
#define	CUBIC_H

#include <glm/glm.hpp>

class cubic {
public:
    cubic();
    cubic(glm::vec3 _a,glm::vec3 _b,glm::vec3 _c,glm::vec3 _d);
    ~cubic();
	/*Bod na splinu*/
    glm::vec3 getPointOnCubicSpline(float t);
    glm::vec3 a, b, c, d;       //Parametry kubického splinu
private:
};

#endif	/* CUBIC_H */

