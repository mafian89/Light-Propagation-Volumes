#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <ctime>
#include <math.h>
#include <vector>
#include <algorithm>    // std::max
#include <fstream>
#include <map>

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/type_ptr.hpp>

#define SHADOWMAPSIZE 2048
#define RSMSIZE 256
#define TEXTUREDIR "../textures/"
#define SRCDIR "../src/"
#define MAX_GRID_SIZE 32
#define VPL_COUNT RSMSIZE*RSMSIZE
#define DEG2RAD (M_PI / 180.0f)

//#define VPL_DEBUG
//#define GRIDS_DEBUG
#define ORTHO_PROJECTION
#define USESAMPLER3D
#define PROPAGATION_STEPS 8
#define CASCADES 3