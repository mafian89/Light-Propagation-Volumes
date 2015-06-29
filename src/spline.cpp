/* 
 * File:   spline.cpp
 * Desc:	Trida zajistuje interpolaci splinu
 */

#include "spline.h"

spline::spline() {
}

spline::~spline() {
    this->keyFrames.clear();
    this->cameraPath.clear();
}

void spline::init() {
    this->interpolStep = 200;
    this->currentCamIndex = 0;
    this->currentKeyFrameCameraIndex = 0;
    this->loadKeyFrames("../misc/keyFrames.txt");
    this->enableCubicInterpolation(true);
    this->buildSplinePath();
}

void spline::enableCubicInterpolation(bool e) {
    this->cubicInterpolation = e;
}

void spline::buildSplinePath() {
    std::cout<<"Spline path building has begun!"<<std::endl;
    if(!this->cubicInterpolation){
        for(unsigned int i = 0; i < this->keyFrames.size()-1; i++){
            for(unsigned int j = 0; j < this->interpolStep; j++) {
                animationCamera * anim_tmp = new animationCamera();

                glm::vec3 difference = this->getSplineKeyFrameOnIndex(i+1)->getAnimationCameraPosition() - this->getSplineKeyFrameOnIndex(i)->getAnimationCameraPosition();
                difference *= (float) j/(float)(this->interpolStep - 1);
                glm::vec3 pos = this->getSplineKeyFrameOnIndex(i)->getAnimationCameraPosition()+difference;

                difference = this->getSplineKeyFrameOnIndex(i+1)->getAnimationCameraDirection() - this->getSplineKeyFrameOnIndex(i)->getAnimationCameraDirection();
                difference *= (float) j/(float)(this->interpolStep - 1);
                glm::vec3 dir = this->getSplineKeyFrameOnIndex(i)->getAnimationCameraDirection()+difference;

                difference = this->getSplineKeyFrameOnIndex(i+1)->getAnimationCameraUp() - this->getSplineKeyFrameOnIndex(i)->getAnimationCameraUp();
                difference *= (float) j/(float)(this->interpolStep - 1);
                glm::vec3 up = this->getSplineKeyFrameOnIndex(i)->getAnimationCameraUp()+difference;

                glm::vec3 right = glm::cross(up,dir);

                anim_tmp->setAnimationCameraRight(right);

//                glm::vec3 u = glm::cross(dir,right);


                anim_tmp->setAnimationCameraAtPosition(pos, 
                                                       dir,            //mam obracene
                                                       up);

                anim_tmp->fov = this->getSplineKeyFrameOnIndex(i)->fov;
                anim_tmp->aspectRatio = this->getSplineKeyFrameOnIndex(i)->aspectRatio;

                anim_tmp->init();

                cameraPath.push_back(anim_tmp);
            }
        }
    } else {    //Cubic
        std::vector<glm::vec3> pos(this->keyFrames.size());
        std::vector<glm::vec3> dir(this->keyFrames.size());
        std::vector<glm::vec3> up(this->keyFrames.size());
        
        for(unsigned int i = 0; i <this->keyFrames.size(); i++) {
            pos[i] = this->getSplineKeyFrameOnIndex(i)->getAnimationCameraPosition();
            dir[i] = this->getSplineKeyFrameOnIndex(i)->getAnimationCameraDirection();
            up[i] = this->getSplineKeyFrameOnIndex(i)->getAnimationCameraUp();
        }
        
        std::vector<cubic> cubic_pos = buildCubicSpline(this->keyFrames.size()-1, pos);
        std::vector<cubic> cubic_dir = buildCubicSpline(this->keyFrames.size()-1, dir);
        std::vector<cubic> cubic_up = buildCubicSpline(this->keyFrames.size()-1, up);
        
        
        for(unsigned int i = 0; i < this->keyFrames.size()-1; i++)
        {
            for(unsigned int j = 0; j < this->interpolStep; j++) 
            {
                animationCamera * anim_tmp = new animationCamera();
                
                float diff = (float)j/(float)(this->interpolStep - 1);
                glm::vec3 _pos = cubic_pos[i].getPointOnCubicSpline(diff);
                glm::vec3 _dir = cubic_dir[i].getPointOnCubicSpline(diff);
                glm::vec3 _up = cubic_up[i].getPointOnCubicSpline(diff);
                glm::vec3 right = glm::cross(_up,_dir);

                anim_tmp->setAnimationCameraRight(right);

                anim_tmp->setAnimationCameraAtPosition(_pos, 
                                                       _dir,            //mam obracene
                                                       _up);

                anim_tmp->fov = this->getSplineKeyFrameOnIndex(i)->fov;
                anim_tmp->aspectRatio = this->getSplineKeyFrameOnIndex(i)->aspectRatio;

                anim_tmp->init();

                cameraPath.push_back(anim_tmp);
            }
        }
    }
	std::cout << "Spline path builded!" << std::endl;
}

// algorithm from: http://www.cse.unsw.edu.au/~lambert/splines/
std::vector<cubic> spline::buildCubicSpline(int size, std::vector<glm::vec3> v) {
    std::vector<glm::vec3> gamma(size+1);
    std::vector<glm::vec3> delta(size+1);
    std::vector<glm::vec3> D(size+1);
    //Solving this:
    //From: http://mathworld.wolfram.com/CubicSpline.html
    //               [2 1       ] [D[0]]   [3(v[1] - v[0])  ]
    //               |1 4 1     | |D[1]|   |3(v[2] - v[0])  |
    //               |  1 4 1   | | .  | = |      .         |
    //               |    ..... | | .  |   |      .         |
    //               |     1 4 1| | .  |   |3(v[n] - v[n-2])|
    //               [       1 2] [D[n]]   [3(v[n] - v[n-1])]
    int i;
//    gamma[0] = glm::vec3(0.0f);
    gamma[0] = glm::vec3(1.0f/2.0f);
//    std::cout<<gamma[0].x<<" "<<gamma[0].y<<" "<<gamma[0].z<<" "<<std::endl;
    for(i = 1; i < size; i++) {
        gamma[i] = glm::vec3(1.0f)/(glm::vec3(4.0f) - gamma[i-1]);
//        std::cout<<gamma[i].x<<" "<<gamma[i].y<<" "<<gamma[i].z<<" "<<std::endl;
    }
    gamma[size] = glm::vec3(1.0f)/(glm::vec3(2.0f) - gamma[size-1]);
//    std::cout<<gamma[size].x<<" "<<gamma[size].y<<" "<<gamma[size].z<<" "<<std::endl;
    
    delta[0] = glm::vec3(3.0f)*(v[1] - v[0])*gamma[0];
    for(i = 1; i < size; i++) {
        delta[i] = ( glm::vec3(3.0f)*(v[i+1]-v[i-1])- delta[i-1] ) * gamma[i];
    }
    delta[size] = (glm::vec3(3.0f)*(v[size] - v[size-1]) - delta[size-1] * gamma[size]);
//    std::cout<<delta[size].x<<" "<<delta[size].y<<" "<<delta[size].z<<" "<<std::endl;
    
    D[size] = delta[size];
    for(i = size - 1; i >= 0; i--) {
        D[i] = delta[i] - gamma[i] * D[i+1];
//        std::cout<<D[i].x<<" "<<D[i].y<<" "<<D[i].z<<" "<<std::endl;
    }
    
    std::vector<cubic> myCubic(size);
    for(i = 0; i < size; i++) {
        cubic tmp(v[i], D[i], glm::vec3(3.0f)*(v[i + 1] - v[i]) - glm::vec3(2.0f)*D[i] - D[i + 1],
                glm::vec3(2.0f)*(v[i] - v[i + 1]) + D[i] + D[i+1]);
        myCubic[i] = tmp;
//        std::cout<<tmp.a.x<<" "<<tmp.a.y<<" "<<tmp.a.z<<std::endl;
    }
    
//    std::cout<<myCubic[2].a.x<<" "<<myCubic[2].a.y<<" "<<myCubic[2].a.z<<std::endl;
    return myCubic;
}

void spline::loadKeyFrames(std::string file) {
    std::fstream myFile;
    myFile.open(file.c_str(), std::fstream::in | std::fstream::app);
    if(myFile.is_open()) {
        std::cout<<"Loading keyFrames from file: "<<file<<std::endl;
        char line[1024];
        while(!myFile.eof()) {
//            std::cout<<line<<std::endl;
            //char str[128];
            myFile.getline(line,1024);
            char str[1024];
            sscanf (line, "%s", str);

            //#p pos.x pos.y pos.z d dir.x dir.y dir.z u up.x up.y up.z f fov a aspec_ratio
            if(strcmp(str, "p")) {
                continue;
            }
            
            animationCamera * anim_tmp = new animationCamera();
            float p_x,p_y,p_z,d_x,d_y,d_z,u_x,u_y,u_z,r_x,r_y,r_z,f,a;/*,ha,va*/;
            sscanf (line,"p %f %f %f d %f %f %f u %f %f %f r %f %f %f f %f a %f",&p_x,&p_y,&p_z,&d_x,&d_y,&d_z,&u_x,&u_y,&u_z,&r_x,&r_y,&r_z,&f,&a/*,&ha,&va*/);

            anim_tmp->setAnimationCameraAtPosition(glm::vec3(p_x,p_y,p_z), glm::vec3(d_x,d_y,d_z), glm::vec3(u_x,u_y,u_z));

            anim_tmp->fov = f;
            anim_tmp->aspectRatio = a;
            
            anim_tmp->init();

            keyFrames.push_back(anim_tmp);
        }
        
        
    } else {
        std::cout<<"Cannot open file: "<<file<<std::endl;
    }
    std::cout<<"KeyFrames were successfully loaded, closing file: "<<file<<" now!"<<std::endl;
    myFile.close();
}

animationCamera * spline::getSplineKeyFrameOnIndex(unsigned int index) {
    return this->keyFrames[index];
}

std::vector<animationCamera *> spline::getSplineKeyFrames(){
    return this->keyFrames;
}

unsigned int spline::getSplineCurrKeyFrameIndex(){
    return this->currentKeyFrameCameraIndex;
}
unsigned int spline::getSplineCurrCamIndex(){
    return this->currentCamIndex;
}

std::vector<animationCamera *> spline::getSplineCameraPath(){
    return this->cameraPath;
}
animationCamera * spline::getSplineCameraPathOnIndex(unsigned int index) {
    return this->cameraPath[index];
}

void spline::setSplineCurrKeyFrameIndex(unsigned int index) {
    this->currentKeyFrameCameraIndex = index;
}

void spline::setSplineCurrCamIndex(unsigned int index){
    this->currentCamIndex = index;
}