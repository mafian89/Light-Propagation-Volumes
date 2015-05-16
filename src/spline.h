/* 
 * File:   spline.h
 * Desc:	Trida zajistuje interpolaci splinu
 */

#ifndef SPLINE_H
#define	SPLINE_H

#include <glm/glm.hpp>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>

#include "animationCamera.h"
#include "cubic.h"

class spline {
public:
    spline();
    ~spline();
    /*Inicializace*/
    void init();
    
	/*Vytvoreni splinu*/
    void buildSplinePath(/*type of interpolation*/);
	/*Nacteni klicovych snimku*/
    void loadKeyFrames(std::string file);
    
	/*Gettery*/
    std::vector<animationCamera *> getSplineKeyFrames();
    animationCamera * getSplineKeyFrameOnIndex(unsigned int index);
    std::vector<animationCamera *> getSplineCameraPath();
    animationCamera * getSplineCameraPathOnIndex(unsigned int index);
    
    unsigned int getSplineCurrKeyFrameIndex();
    unsigned int getSplineCurrCamIndex();
    
	/*Settery*/
    void setSplineCurrKeyFrameIndex(unsigned int index);
    void setSplineCurrCamIndex(unsigned int index);
    void enableCubicInterpolation(bool e);
    std::vector<cubic> buildCubicSpline(int size, std::vector<glm::vec3> v);
    unsigned int interpolStep;
private:
    unsigned int currentCamIndex;
    unsigned int currentKeyFrameCameraIndex;
    bool cubicInterpolation;
    std::vector<animationCamera *> keyFrames;
    std::vector<animationCamera *> cameraPath;
};

#endif	/* SPLINE_H */

