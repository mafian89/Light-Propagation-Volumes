/* 
 * File:   cubic.cpp
 * Desc: Trida, pro kubicky spline
 */

#include "cubic.h"

cubic::cubic() {
}

cubic::cubic(glm::vec3 _a,glm::vec3 _b,glm::vec3 _c,glm::vec3 _d) {
    this->a = _a;
    this->b = _b;
    this->c = _c;
    this->d = _d;
}

glm::vec3 cubic::getPointOnCubicSpline(float t) {
    //Yi(t) = ai + bi*t + ci*t^2 + di*t^3
    //return a + b*t + c*t*t + d*t*t*t;
    return (((d*t) + c)*t + b)*t + a;        //should be faster
}

cubic::~cubic() {
}

