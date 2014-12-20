#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <ctime>
#include <math.h>
#include <vector>

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

#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/type_ptr.hpp>

#define SHADOWMAPSIZE 1024