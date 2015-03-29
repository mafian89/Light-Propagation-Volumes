#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <ctime>
#include <math.h>
#include <vector>
#include <algorithm>    // std::max

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

#include "GLSLShader/GLSLShader.h"
#include "camera/controlCamera.h"
#include "Mesh/Mesh.h"
#include "fboManager.h"
#include "textureManager.h"
#include "CLightObject.h"
#include "texUtils.h"
#include "CBoundingBox.h"
#include "DebugDrawer.h"

#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/type_ptr.hpp>

#define SHADOWMAPSIZE 2048
#define RSMSIZE 512
#define TEXTUREDIR "../textures/"
#define MAX_GRID_SIZE 64
#define VPL_COUNT RSMSIZE*RSMSIZE
#define DEG2RAD (M_PI / 180.0f)

#define VPL_DEBUG
//#define ORTHO_PROJECTION
//#define LAYERED_FILL