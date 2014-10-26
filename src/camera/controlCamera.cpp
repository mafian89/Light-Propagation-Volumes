#include "controlCamera.h"

CControlCamera::CControlCamera() {
}

CControlCamera::~CControlCamera() {
}

glm::mat4 CControlCamera::getViewMatrix(){
        return this->ViewMatrix;
}
glm::mat4 CControlCamera::getProjectionMatrix(){
        return this->ProjectionMatrix;
}

glm::vec3 CControlCamera::getPosition(){
	return this->position;
}

glm::vec3 CControlCamera::getInitialPosition(){
	return this->initial_position;
}

glm::vec3 CControlCamera::getDirection(){
	return this->direction;
}

glm::vec3 CControlCamera::getRight(){
	return this->right;
}

glm::vec3 CControlCamera::getUp(){
	return this->up;
}

float CControlCamera::getHorizontalAngle(){
	return this->horizontalAngle;
}

float CControlCamera::getInitialHorizontalAngle(){
	return this->initial_horizontalAngle;
}

float CControlCamera::getVerticalAngle(){
	return this->verticalAngle;
}

float CControlCamera::getInitialVerticalAngle(){
	return this->initial_verticalAngle;
}

//--Settery
void CControlCamera::setHorizontalAngle(float _horAngle){
	this->horizontalAngle = _horAngle;
}

void CControlCamera::setVerticalAngle(float _vertAngle){
	this->verticalAngle = _vertAngle;
}

void CControlCamera::setPosition(glm::vec3 _pos){
	this->position = _pos;
}

void CControlCamera::setDirection(glm::vec3 _dir){
	this->direction = _dir;
}

void CControlCamera::setUp(glm::vec3 _up){
	this->up;
}

void CControlCamera::setWindow(SDL_Window * w) {
	this->window = w;
}

void CControlCamera::initControlCamera(glm::vec3 _pos, SDL_Window * w, float _horAngle = 3.14f, float _verAngle = 0.0f, 
										unsigned _width = 800, unsigned _height = 600, float ncp = 0.1f, float fcp = 1000.0) {
    this->position = _pos;
    this->initial_position = this->position;
    this->horizontalAngle = _horAngle;
    this->initial_horizontalAngle = horizontalAngle;
    this->verticalAngle = _verAngle;
    this->initial_verticalAngle = this->verticalAngle;
    this->fov = 45.0f;
    this->aspec = (float)_width/(float)_height;
	this->width = _width;
	this->height = _height;
	this->far_clip_plane = fcp;
	this->near_clip_plane = ncp;
	this->mouseSpeed = 0.0005;
	this->moved = false;
	this->window = w;
}

void CControlCamera::computeMatricesFromInputs(void){
		if(moved) {
			SDL_GetMouseState(&_x,&_y); 
			//SDL_WarpMouse((Uint16)(width/2),(Uint16)(height/2));
			SDL_WarpMouseInWindow(window, (Uint16)(width / 2), (Uint16)(height / 2));
			this->horizontalAngle += this->mouseSpeed *  (float(this->width/2) - float(this->_x));
			this->verticalAngle += + this->mouseSpeed*  (float(this->height/2) - float(this->_y));
			if(this->verticalAngle >= 3.14f/2.0f){
				this->verticalAngle = (3.14f/2.0f);
			} else if(verticalAngle <= (-3.14f/2.0f)){
				this->verticalAngle = (-3.14f/2.0f);
			}
		}
        // Direction : Spherical coordinates to Cartesian coordinates conversion
        //Viz http://www.lighthouse3d.com/wp-content/uploads/2011/04/vfpoints2.gif
        this->direction = glm::vec3(
                cos(this->verticalAngle) * sin(this->horizontalAngle), 
                sin(this->verticalAngle),
                cos(this->verticalAngle) * cos(this->horizontalAngle)
        );
        
        // Right vector
        this->right = glm::vec3(
                sin(this->horizontalAngle - 3.14f/2.0f), 
                0,
                cos(this->horizontalAngle - 3.14f/2.0f)
        );
        
        // Up vector - cross produktem dostanu kolmý vektor na tyto dva
        this->up = glm::cross( right, direction );
#ifdef DEBUG
        std::cout<<"Camera POSITION vector: ("<<position.x<<", "<<position.y<<", "<<position.z<<")"<<std::endl;
        std::cout<<"Camera UP vector: ("<<up.x<<", "<<up.y<<", "<<up.z<<")"<<std::endl;
        std::cout<<"Camera RIGHT vector: ("<<right.x<<", "<<right.y<<", "<<right.z<<")"<<std::endl;
        std::cout<<"Camera DIRECTION vector: ("<<direction.x<<", "<<direction.y<<", "<<direction.z<<")"<<std::endl;
		std::cout<<"Camera horizotnal angle: "<<horizontalAngle<<std::endl;
		std::cout<<"Camera vertical angle: "<<verticalAngle<<std::endl;
#endif
        this->aspec = (float)width/(float)height;
		this->ProjectionMatrix = glm::perspective(fov, aspec, this->near_clip_plane, this->far_clip_plane);
        // Camera matrix
        this->ViewMatrix       = glm::lookAt(
												this->position,           
												this->position+this->direction,
												this->up                 
											);
//        ViewMatrix = glm::rotate(ViewMatrix, -90.0f, glm::vec3(1,0,0)); 
        
}


