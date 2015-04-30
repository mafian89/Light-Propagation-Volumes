#include "common.h"
#include "CTextureViewer.h"


#ifdef _MSC_VER
#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "glew32.lib")

#pragma comment(lib,"devil.lib")
#pragma comment(lib,"ILU.lib")
//#pragma comment(lib,"ILUT.lib")

#pragma comment(lib,"assimp.lib")
/// etc
#endif

//error LNK2019: unresolved external symbol _main referenced in function ___tmainCRTStartup
#undef main
//or use sdlmain.lib

using namespace std;

#define WIDTH 800
#define HEIGHT 600

float aspect;
CTextureViewer * ctv;
CTextureViewer * ctv2;
CControlCamera * controlCamera = new CControlCamera();
GLSLShader basicShader, rsmShader, shadowMap, injectLight, injectLight_layered, VPLsDebug, geometryInject, geometryInject_layered, gBufferShader, propagationShader, propagationShader_layered;
Mesh * mesh;
GBuffer * gBuffer;
float movementSpeed = 10.0f;
float ftime;
//glm::vec3 lightPosition(0.0, 4.0, 2.0);
CTextureManager texManager;
CFboManager * fboManager = new CFboManager();
CFboManager * RSMFboManager = new CFboManager();
CFboManager * ShadowMapManager = new CFboManager();
CFboManager * lightInjectFBO = new CFboManager();
CFboManager * geometryInjectFBO = new CFboManager();
CFboManager * propagationFBO = new CFboManager(); 
CLightObject * light;
DebugDrawer * dd;
//GLuint depthPassFBO;
GLint texture_units, max_color_attachments;
GLuint VPLsVAO, VPLsVBO, PropagationVAO, PropagationVBO;
glm::vec3 volumeDimensions, vMin, editedVolumeDimensions;
float cellSize;
float f_tanFovXHalf;
float f_tanFovYHalf;
float f_texelAreaModifier = 1.0f; //Arbitrary value
bool b_useNormalOffset = false;
bool b_firstPropStep = true;
bool b_useOcclusion = true;
float f_indirectAttenuation = 0.8f;
bool b_useLayeredFill = true;

int volumeDimensionsMult;
bool useMultiStepPropagation = false;

glm::mat4 biasMatrix(
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 0.5, 0.0,
	0.5, 0.5, 0.5, 1.0
	);

typedef struct propTex {
	GLuint red, green, blue;
} propTextureType;

propTextureType propTextures[PROPAGATION_STEPS];

//#define CTV
//#define W2

void initializeVPLsInvocations() {
	////////////////////////////////////////////////////
	// VPL INIT STUFF
	////////////////////////////////////////////////////
	injectLight.Use();
	//Generate VAO
	glGenVertexArrays(1, &VPLsVAO);

	//Bind VAO
	glBindVertexArray(VPLsVAO);

	//Generate VBO
	glGenBuffers(1, &VPLsVBO);
	//Bind VBO
	glBindBuffer(GL_ARRAY_BUFFER, VPLsVBO);

	float *testPoints = new float[2 * VPL_COUNT];
	float step = 1.0 / VPL_COUNT;
	for (int i = 0; i < VPL_COUNT; ++i) {
		testPoints[i * 2] = 0.0f;
		testPoints[i * 2 + 1] = 0.0f;
	}

	//Alocate buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(testPoints), testPoints, GL_STATIC_DRAW);
	//Fill VBO
	//glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(testPoints), testPoints);

	//Fill attributes and uniforms
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, (sizeof(float)* 2), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	injectLight.UnUse();

	//Free memory
	delete testPoints;
}

typedef struct pi {
	float r, c, d;
} PROP_ITEM;

void initializePropagationVAO(glm::vec3 volumeDimensions) {
	propagationShader.Use();

	//Generate VAO
	glGenVertexArrays(1, &PropagationVAO);

	//Bind VAO
	glBindVertexArray(PropagationVAO);

	//Generate VBO
	glGenBuffers(1, &PropagationVBO);
	//Bind VBO
	glBindBuffer(GL_ARRAY_BUFFER, PropagationVBO);

	volumeDimensionsMult = volumeDimensions.x * volumeDimensions.y * volumeDimensions.z;
	int x = volumeDimensions.x, y = volumeDimensions.y, z = volumeDimensions.z;
	/*float *testPoints = new float[3 * count];
	for (int i = 0; i < count; ++i) {
		testPoints[i * 3] = 0.0f;
		testPoints[i * 3 + 1] = 0.0f;
		testPoints[i * 3 + 2] = 0.0f;
	}*/

	std::vector<glm::vec3> coords;
	for (int d = 0; d < z; d++) {
		for (int c = 0; c < y; c++) {
			for (int r = 0; r < x; r++) {
				coords.push_back(glm::vec3((float)r, (float)c, (float)d));
			}
		}
	}

	//std::cout << coords.size() * 3 * sizeof(float) << std::endl;
	//std::cout << coords.size() * sizeof(glm::vec3) << std::endl;

	//Alocate buffer
	glBufferData(GL_ARRAY_BUFFER, coords.size() * 3 * sizeof(float), &coords.front(), GL_STATIC_DRAW);
	//Fill VBO
	//glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(testPoints), testPoints);

	//Fill attributes and uniforms
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (sizeof(float)* 3), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	propagationShader.UnUse();
	//delete testPoints;

	/*for (int i = 0; i < coords.size(); i++) {
		std::cout << coords[i].x << " " << coords[i].y << " " << coords[i].z << std::endl;
	}*/
	//delete tmp;
}

//This function *MUST* be called after creation of LightGrid texture
void initPropStepTextures() {
	propTextures[0].red = texManager["LPVGridR"];
	propTextures[0].green = texManager["LPVGridG"];
	propTextures[0].blue = texManager["LPVGridB"];
	for (int i = 1; i < PROPAGATION_STEPS; i++) {
		string texNameR = "RLPVStep" + std::to_string(i);
		string texNameG = "GLPVStep" + std::to_string(i);
		string texNameB = "BLPVStep" + std::to_string(i);
		//std::cout << texName << std::endl;
		texManager.createRGBA16F3DTexture(texNameR, volumeDimensions, GL_NEAREST, GL_CLAMP_TO_EDGE);
		texManager.createRGBA16F3DTexture(texNameG, volumeDimensions, GL_NEAREST, GL_CLAMP_TO_EDGE);
		texManager.createRGBA16F3DTexture(texNameB, volumeDimensions, GL_NEAREST, GL_CLAMP_TO_EDGE);
		propTextures[i].red = texManager[texNameR];
		propTextures[i].green = texManager[texNameG];
		propTextures[i].blue = texManager[texNameB];
	}
}

void Initialize(SDL_Window * w) {
	//glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &texture_units);
	//Image uniforms GL_MAX_COMBINED_IMAGE_UNIFORMS - combined
	//glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &max_color_attachments);
	//std::cout << "Max color attachments: " << max_color_attachments << std::endl;
	//tex = loadImage("../textures/texture.png");


#ifdef ORTHO_PROJECTION
	/*
	Light POSITION vector: (-7.07759, 56.7856, 10.0773)
	Light DIRECTION vector: (0.168037, -0.964752, -0.202527)
	Light horizotnal angle: 2.449
	Light vertical angle: -1.3045

	Light POSITION vector: (-0.197587, 65.0856, 10.0773)
	Light DIRECTION vector: (0.000831289, -0.947236, -0.320536)
	Light horizotnal angle: 3.139
	Light vertical angle: -1.2445
	*/
	light = new CLightObject(glm::vec3(-0.197587, 65.0856, 10.0773), glm::vec3(0.000831289, -0.947236, -0.320536));
	light->setHorAngle(3.139);
	light->setVerAngle(-1.2445);
#else
	/*
	Light POSITION vector: (-0.977592, 59.4256, 10.0773)
	Light DIRECTION vector: (0.00145602, -0.827421, -0.56158)
	Light horizotnal angle: 3.139
	Light vertical angle: -0.9745
	*/
	light = new CLightObject(glm::vec3(-0.977592, 59.4256, 10.0773), glm::vec3(0.00145602, -0.827421, -0.56158));
	light->setHorAngle(3.139);
	light->setVerAngle(-0.9745);
#endif

	ctv = new CTextureViewer(0, "../shaders/textureViewer.vs", "../shaders/textureViewer.frag");
	ctv2 = new CTextureViewer(0, "../shaders/textureViewer.vs", "../shaders/textureViewer.frag");

	////////////////////////////////////////////////////
	// SHADERS INIT
	////////////////////////////////////////////////////
	basicShader.LoadFromFile(GL_VERTEX_SHADER, std::string("../shaders/basicShader.vs").c_str());
	basicShader.LoadFromFile(GL_FRAGMENT_SHADER, std::string("../shaders/basicShader.frag").c_str());
	basicShader.CreateAndLinkProgram();

	rsmShader.LoadFromFile(GL_VERTEX_SHADER, std::string("../shaders/RSMpass.vs").c_str());
	rsmShader.LoadFromFile(GL_FRAGMENT_SHADER, std::string("../shaders/RSMpass.frag").c_str());
	rsmShader.CreateAndLinkProgram();

	shadowMap.LoadFromFile(GL_VERTEX_SHADER, std::string("../shaders/depthOnly.vs").c_str());
	shadowMap.LoadFromFile(GL_FRAGMENT_SHADER, std::string("../shaders/depthOnly.frag").c_str());
	shadowMap.CreateAndLinkProgram();

	gBufferShader.LoadFromFile(GL_VERTEX_SHADER, std::string("../shaders/gbufferFill.vs").c_str());
	gBufferShader.LoadFromFile(GL_FRAGMENT_SHADER, std::string("../shaders/gbufferFill.frag").c_str());
	gBufferShader.CreateAndLinkProgram();


	propagationShader.LoadFromFile(GL_VERTEX_SHADER, std::string("../shaders/propagation.vs").c_str());
	propagationShader.LoadFromFile(GL_FRAGMENT_SHADER, std::string("../shaders/propagation.frag").c_str());
	propagationShader.CreateAndLinkProgram();


	injectLight_layered.LoadFromFile(GL_VERTEX_SHADER, std::string("../shaders/lightInject_layered.vs").c_str());
	injectLight_layered.LoadFromFile(GL_GEOMETRY_SHADER, std::string("../shaders/lightInject_layered.gs").c_str());
	injectLight_layered.LoadFromFile(GL_FRAGMENT_SHADER, std::string("../shaders/lightInject_layered.frag").c_str());
	injectLight_layered.CreateAndLinkProgram();

	injectLight.LoadFromFile(GL_VERTEX_SHADER, std::string("../shaders/lightInject.vs").c_str());
	injectLight.LoadFromFile(GL_FRAGMENT_SHADER, std::string("../shaders/lightInject.frag").c_str());
	injectLight.CreateAndLinkProgram();

	geometryInject.LoadFromFile(GL_VERTEX_SHADER, std::string("../shaders/geometryInject.vs").c_str());
	geometryInject.LoadFromFile(GL_FRAGMENT_SHADER, std::string("../shaders/geometryInject.frag").c_str());
	geometryInject.CreateAndLinkProgram();

	
	geometryInject_layered.LoadFromFile(GL_VERTEX_SHADER, std::string("../shaders/geometryInject_layered.vs").c_str());
	geometryInject_layered.LoadFromFile(GL_GEOMETRY_SHADER, std::string("../shaders/geometryInject_layered.gs").c_str());
	geometryInject_layered.LoadFromFile(GL_FRAGMENT_SHADER, std::string("../shaders/geometryInject_layered.frag").c_str());
	geometryInject_layered.CreateAndLinkProgram();
	/*
	propagationShader_layered.LoadFromFile(GL_VERTEX_SHADER, std::string("../shaders/propagation_layered.vs").c_str());
	propagationShader_layered.LoadFromFile(GL_GEOMETRY_SHADER, std::string("../shaders/propagation_layered.gs").c_str());
	propagationShader_layered.LoadFromFile(GL_FRAGMENT_SHADER, std::string("../shaders/propagation_layered.frag").c_str());
	propagationShader_layered.CreateAndLinkProgram();
	*/
#ifdef VPL_DEBUG
	VPLsDebug.LoadFromFile(GL_VERTEX_SHADER, std::string("../shaders/debugVPLs.vs").c_str());
	VPLsDebug.LoadFromFile(GL_FRAGMENT_SHADER, std::string("../shaders/debugVPLs.frag").c_str());
	VPLsDebug.CreateAndLinkProgram();
#endif

	////////////////////////////////////////////////////
	// CAMERA INIT
	////////////////////////////////////////////////////
	/*
	Camera POSITION vector: (11.7542, 14.1148, 0.822185)
	Camera UP vector: (-0.436604, 0.873719, -0.214456)
	Camera RIGHT vector: (0.440876, 0, -0.897568)
	Camera DIRECTION vector: (-0.783916, -0.486431, -0.385826)
	Camera horizotnal angle: 4.25502
	Camera vertical angle: -0.508

	//Blocking view
	Camera POSITION vector: (10.7061, 12.0907, -8.37268)
	Camera UP vector: (-0.410051, 0.907291, 0.0931737)
	Camera RIGHT vector: (-0.221577, 0, -0.975143)
	Camera DIRECTION vector: (-0.884898, -0.420503, 0.20033)
	Camera horizotnal angle: 4.93502
	Camera vertical angle: -0.434

	Camera POSITION vector: (5.95956, 10.9459, -0.109317)
	Camera UP vector: (-0.348451, 0.93519, -0.063252)
	Camera RIGHT vector: (0.178604, 0, -0.983921)
	Camera DIRECTION vector: (-0.92002, -0.354145, -0.167762)
	Camera horizotnal angle: 4.53202
	Camera vertical angle: -0.362
	*/
	//Normal camera
	//controlCamera->initControlCamera(glm::vec3(11.7542, 14.1148, 0.822185), w, 4.25502, -0.508, WIDTH, HEIGHT, 1.0, 1000.0);
	//Blocking view
	//controlCamera->initControlCamera(glm::vec3(10.7061, 12.0907, -8.37268), w, 4.93502, -0.434, WIDTH, HEIGHT, 1.0, 1000.0);
	//Debug
	controlCamera->initControlCamera(glm::vec3(5.95956, 10.9459, -0.109317), w, 4.53202, -0.362, WIDTH, HEIGHT, 1.0, 1000.0);

	////////////////////////////////////////////////////
	// UNIFORMS/ATTRIBUTES SETUP
	////////////////////////////////////////////////////
	basicShader.Use();
	basicShader.AddUniform("mvp");
	basicShader.AddUniform("mv");
	basicShader.AddUniform("v");
	basicShader.AddUniform("mn");
	basicShader.AddUniform("vLightPos");
	basicShader.AddUniform("shadowMatrix");
	basicShader.AddUniform("depthTexture");
	//basicShader.AddUniform("AccumulatorLPV");
	basicShader.AddUniform("RAccumulatorLPV");
	basicShader.AddUniform("GAccumulatorLPV");
	basicShader.AddUniform("BAccumulatorLPV");
	basicShader.AddUniform("f_cellSize");
	basicShader.AddUniform("v_gridDim");
	basicShader.AddUniform("v_min");
	basicShader.AddUniform("f_indirectAttenuation");
	basicShader.UnUse();

	rsmShader.Use();
	rsmShader.AddUniform("mvp");
	rsmShader.AddUniform("m");
	rsmShader.AddUniform("mn");
	rsmShader.AddUniform("mv");
	rsmShader.AddUniform("v_lightPos");
	rsmShader.UnUse();

	shadowMap.Use();
	shadowMap.AddUniform("mvp");
	shadowMap.UnUse();

	gBufferShader.Use();
	gBufferShader.AddUniform("mvp");
	gBufferShader.AddUniform("mn");
	gBufferShader.AddUniform("mv");
	gBufferShader.AddUniform("colorTex");
	gBufferShader.UnUse();

	injectLight.Use();
	injectLight.AddUniform("LPVGridR");
	injectLight.AddUniform("LPVGridG");
	injectLight.AddUniform("LPVGridB");
	injectLight.AddUniform("v_gridDim");
	injectLight.AddUniform("f_cellSize");
	injectLight.AddUniform("v_min");
	injectLight.AddUniform("i_RSMsize");
	injectLight.AddUniform("rsm_world_space_coords_tex");
	injectLight.AddUniform("rsm_normal_tex");
	injectLight.AddUniform("rsm_flux_tex");
	injectLight.UnUse();
	//b_useLayeredFill

	injectLight_layered.Use();
	injectLight_layered.AddUniform("LPVGridR");
	injectLight_layered.AddUniform("LPVGridG");
	injectLight_layered.AddUniform("LPVGridB");
	injectLight_layered.AddUniform("v_gridDim");
	injectLight_layered.AddUniform("f_cellSize");
	injectLight_layered.AddUniform("v_min");
	injectLight_layered.AddUniform("i_RSMsize");
	injectLight_layered.AddUniform("rsm_world_space_coords_tex");
	injectLight_layered.AddUniform("rsm_normal_tex");
	injectLight_layered.AddUniform("rsm_flux_tex");
	injectLight_layered.UnUse();

	geometryInject.Use();
	geometryInject.AddUniform("GeometryVolume");
	geometryInject.AddUniform("v_gridDim");
	geometryInject.AddUniform("f_cellSize");
	geometryInject.AddUniform("v_min");
	geometryInject.AddUniform("i_RSMsize");
	geometryInject.AddUniform("rsm_world_space_coords_tex");
	geometryInject.AddUniform("rsm_normal_tex");
	geometryInject.AddUniform("rsm_flux_tex");
	geometryInject.AddUniform("f_tanFovXHalf");
	geometryInject.AddUniform("f_tanFovYHalf");
	geometryInject.AddUniform("v_lightPos");
	geometryInject.AddUniform("m_lightView");
	geometryInject.AddUniform("f_texelAreaModifier");
	geometryInject.UnUse();

#ifdef VPL_DEBUG
	VPLsDebug.Use();
	VPLsDebug.AddUniform("rsm_world_space_coords_tex");
	VPLsDebug.AddUniform("rsm_normal_tex");
	VPLsDebug.AddUniform("mvp");
	VPLsDebug.AddUniform("i_RSMsize");
	VPLsDebug.AddUniform("b_useNormalOffset");
	VPLsDebug.UnUse();
#endif

	propagationShader.Use();
	//propagationShader.AddUniform("AccumulatorLPV");
	propagationShader.AddUniform("RAccumulatorLPV");
	propagationShader.AddUniform("GAccumulatorLPV");
	propagationShader.AddUniform("BAccumulatorLPV");
	propagationShader.AddUniform("GeometryVolume");
	propagationShader.AddUniform("RLightGridForNextStep");
	propagationShader.AddUniform("GLightGridForNextStep");
	propagationShader.AddUniform("BLightGridForNextStep");
	propagationShader.AddUniform("LPVGridR");
	propagationShader.AddUniform("LPVGridG");
	propagationShader.AddUniform("LPVGridB");
	propagationShader.AddUniform("b_firstPropStep");
	propagationShader.AddUniform("v_gridDim");
	propagationShader.AddUniform("b_useOcclusion");
	propagationShader.UnUse();

	
	geometryInject_layered.Use();
	geometryInject_layered.AddUniform("GeometryVolume");
	geometryInject_layered.AddUniform("v_gridDim");
	geometryInject_layered.AddUniform("f_cellSize");
	geometryInject_layered.AddUniform("v_min");
	geometryInject_layered.AddUniform("i_RSMsize");
	geometryInject_layered.AddUniform("rsm_world_space_coords_tex");
	geometryInject_layered.AddUniform("rsm_normal_tex");
	geometryInject_layered.AddUniform("rsm_flux_tex");
	geometryInject_layered.AddUniform("f_tanFovXHalf");
	geometryInject_layered.AddUniform("f_tanFovYHalf");
	geometryInject_layered.AddUniform("v_lightPos");
	geometryInject_layered.AddUniform("m_lightView");
	geometryInject_layered.AddUniform("f_texelAreaModifier");
	geometryInject_layered.UnUse();
	/*
	propagationShader_layered.Use();
	//propagationShader.AddUniform("AccumulatorLPV");
	propagationShader_layered.AddUniform("RAccumulatorLPV");
	propagationShader_layered.AddUniform("GAccumulatorLPV");
	propagationShader_layered.AddUniform("BAccumulatorLPV");
	propagationShader_layered.AddUniform("GeometryVolume");
	propagationShader_layered.AddUniform("RLightGridForNextStep");
	propagationShader_layered.AddUniform("GLightGridForNextStep");
	propagationShader_layered.AddUniform("BLightGridForNextStep");
	propagationShader_layered.AddUniform("LPVGridR");
	propagationShader_layered.AddUniform("LPVGridG");
	propagationShader_layered.AddUniform("LPVGridB");
	propagationShader_layered.AddUniform("b_firstPropStep");
	propagationShader_layered.AddUniform("v_gridDim");
	propagationShader_layered.AddUniform("b_useOcclusion");
	propagationShader_layered.UnUse();
	*/

	////////////////////////////////////////////////////
	// LOAD MODELS & FILL THE VARIABLES
	////////////////////////////////////////////////////
	mesh = new Mesh("../models/sponza.obj");
	dd = new DebugDrawer(GL_LINE_STRIP, &(mesh->getBoundingBox()->getDebugDrawPoints()), NULL, NULL);
	volumeDimensions = mesh->getBoundingBox()->getDimensions();
	editedVolumeDimensions = glm::vec3(volumeDimensions.x, volumeDimensions.y * 3, volumeDimensions.z);
	cellSize = mesh->getBoundingBox()->getCellSize();
	std::cout << "Grid size: " << volumeDimensions.x << "x" << volumeDimensions.y << "x" << volumeDimensions.z << " cell size: " << cellSize << std::endl;
	vMin = mesh->getBoundingBox()->getMin();
	initializeVPLsInvocations();
	initializePropagationVAO(volumeDimensions);

	float f_lightFov = light->getFov(); //in degrees, one must convert to radians
	float f_lightAspect = light->getAspectRatio();

	f_tanFovXHalf = tanf(0.5 * f_lightFov * DEG2RAD);
	f_tanFovYHalf = tanf(0.5 * f_lightFov * DEG2RAD)*f_lightAspect; //Aspect is always 1, but just for sure

	gBuffer = new GBuffer(*(&texManager), WIDTH, HEIGHT);


	//glm::vec3 o = (glm::vec3(11.7542, 14.1148, 0.822185) - vMin) / cellSize;
	//std::cout << o.x << " " << o.y << " " << o.z << std::endl;
	//std::vector<glm::vec3> p;
	//p.push_back(glm::vec3(-1.0, 1.0, 1.0f));
	//p.push_back(glm::vec3(1.0, 1.0, 1.0f));
	//p.push_back(glm::vec3(-1.0, -1.0, 1.0));
	//p.push_back(glm::vec3(1.0, -1.0, 1.0));
	//dd = new DebugDrawer(GL_POINTS, &(p), NULL, NULL);

	////////////////////////////////////////////////////
	// TEXTURE INIT
	////////////////////////////////////////////////////
	texManager.createTexture("render_tex", "", WIDTH, HEIGHT, GL_NEAREST, GL_RGBA16F, GL_RGBA, false);
	texManager.createTexture("rsm_normal_tex", "", RSMSIZE, RSMSIZE, GL_NEAREST, GL_RGBA16F, GL_RGBA, false);
	texManager.createTexture("rsm_world_space_coords_tex", "", RSMSIZE, RSMSIZE, GL_NEAREST, GL_RGBA16F, GL_RGBA, false);
	texManager.createTexture("rsm_flux_tex", "", RSMSIZE, RSMSIZE, GL_NEAREST, GL_RGBA16F, GL_RGBA, false);
	texManager.createTexture("rsm_depth_tex", "", SHADOWMAPSIZE, SHADOWMAPSIZE, GL_LINEAR, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, true);
	
	texManager.createRGBA16F3DTexture("LPVGridR", volumeDimensions, GL_NEAREST, GL_CLAMP_TO_EDGE);
	texManager.createRGBA16F3DTexture("LPVGridG", volumeDimensions, GL_NEAREST, GL_CLAMP_TO_EDGE);
	texManager.createRGBA16F3DTexture("LPVGridB", volumeDimensions, GL_NEAREST, GL_CLAMP_TO_EDGE);
	texManager.createRGBA16F3DTexture("GeometryVolume", volumeDimensions, GL_NEAREST, GL_CLAMP_TO_EDGE);
	//texManager.createRGBA16F3DTexture("AccumulatorLPV", editedVolumeDimensions, GL_LINEAR, GL_CLAMP_TO_EDGE);
	texManager.createRGBA16F3DTexture("RAccumulatorLPV", volumeDimensions, GL_LINEAR, GL_CLAMP_TO_EDGE);
	texManager.createRGBA16F3DTexture("GAccumulatorLPV", volumeDimensions, GL_LINEAR, GL_CLAMP_TO_EDGE);
	texManager.createRGBA16F3DTexture("BAccumulatorLPV", volumeDimensions, GL_LINEAR, GL_CLAMP_TO_EDGE);

	initPropStepTextures();
	//texManager.createRGBA3DTexture("test3D", 5, 5, 5, GL_NEAREST, GL_CLAMP_TO_EDGE);
	//texManager.createTexture("rsm_depth_tex", "", WIDTH, HEIGHT, GL_LINEAR, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, true);

	////////////////////////////////////////////////////
	// FBO INIT
	////////////////////////////////////////////////////
	fboManager->initFbo();
	fboManager->genRenderDepthBuffer(WIDTH, HEIGHT);
	fboManager->bindRenderDepthBuffer();
	fboManager->bindToFbo(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texManager["render_tex"]);
	//fboManager->bindToFbo(GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, texManager["rsm_normal_tex"]);
	//fboManager->bindToFbo(GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D,texManager["rsm_depth_tex"]);
	fboManager->setDrawBuffers();
	if (!fboManager->checkFboStatus()){
		return;
	}

	RSMFboManager->initFbo();
	RSMFboManager->genRenderDepthBuffer(WIDTH, HEIGHT);
	RSMFboManager->bindRenderDepthBuffer();
	RSMFboManager->bindToFbo(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texManager["rsm_world_space_coords_tex"]);
	RSMFboManager->bindToFbo(GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, texManager["rsm_normal_tex"]);
	RSMFboManager->bindToFbo(GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, texManager["rsm_flux_tex"]);
	//RSMFboManager->bindToFbo(GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texManager["rsm_depth_tex"]);
	RSMFboManager->setDrawBuffers();
	if (!RSMFboManager->checkFboStatus()){
		return;
	}

	ShadowMapManager->initFbo();
	ShadowMapManager->bindToFbo(GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texManager["rsm_depth_tex"]);
	ShadowMapManager->setDrawBuffers();
	if (!ShadowMapManager->checkFboStatus()) {
		return;
	}

	lightInjectFBO->initFbo();
	//lightInjectFBO->genRenderDepthBuffer(WIDTH, HEIGHT);
	//lightInjectFBO->bindRenderDepthBuffer();
	lightInjectFBO->bind3DTextureToFbo(GL_COLOR_ATTACHMENT0, texManager["LPVGridR"]);
	lightInjectFBO->bind3DTextureToFbo(GL_COLOR_ATTACHMENT1, texManager["LPVGridG"]);
	lightInjectFBO->bind3DTextureToFbo(GL_COLOR_ATTACHMENT2, texManager["LPVGridB"]);
	lightInjectFBO->setDrawBuffers();
	if (!lightInjectFBO->checkFboStatus()) {
		return;
	}

	geometryInjectFBO->initFbo();
	geometryInjectFBO->bind3DTextureToFbo(GL_COLOR_ATTACHMENT0,texManager["GeometryVolume"]);
	geometryInjectFBO->setDrawBuffers();
	if (!geometryInjectFBO->checkFboStatus()) {
		return;
	}

	//IN CASE OF PROBLEMS UNCOMMENT LINE BELOW
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
float rot = 0.0;
float elevation = 0.0;
float rotSpeed = 30.0f;
float elevationSpeed = 1.0f;
bool test = true;
void Display() {
	//Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Clear color
	glClearColor(0.0, 0.0, 0.0, 1.0);
	//Enable depth testing
	glEnable(GL_DEPTH_TEST);
	//View port
	//glViewport(0, 0, WIDTH, HEIGHT);
	//downsample
	//glViewport(0,0,width/2,height/2);

	//Camera update
	controlCamera->computeMatricesFromInputs();

#ifdef ROT
	rot = rot + rotSpeed * ftime;
	if (rot > 360.0)
		rot = 0.0;
	//elevation = elevation + elevationSpeed * ftime;
	if (elevation <= 1.0 && elevation > 0.0) {
		elevation = elevation + elevationSpeed * ftime;
	}
	else {
		elevation = elevation - elevationSpeed * ftime;
	}
#endif

	glm::mat4 m = glm::mat4(1.0f);
#ifdef ROT
	m = glm::rotate(m, rot, glm::vec3(0, 1, 0));
	m = glm::translate(m, glm::vec3(0, sin(elevation), 0));
#endif
	//m = glm::scale(m, glm::vec3(5.0f));
	//glm::mat4 m = glm::mat4(1.0f);
	glm::mat4 v = controlCamera->getViewMatrix();
	glm::mat3 mn = glm::transpose(glm::inverse(glm::mat3(v*m)));
	glm::mat4 mvp = controlCamera->getProjectionMatrix() * v * m;
	glm::mat4 mv = controlCamera->getViewMatrix() * m;

	glm::mat4 v_light = light->getViewMatrix();
	glm::mat4 p_light = light->getProjMatrix();
	glm::mat4 mvp_light = p_light * v_light * m;
	glm::mat4 inverse_vLight = glm::inverse(v_light);
	glm::mat3 mn_light = glm::transpose(glm::inverse(glm::mat3(v_light*m)));

	glm::vec3 lightPosition = light->getPosition();
	/*
	////////////////////////////////////////////////////
	// FILL THE G-BUFFER
	////////////////////////////////////////////////////
	gBuffer->bindToRender();
	glViewport(0, 0, WIDTH, HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gBufferShader.Use();
	glUniform1i(gBufferShader("colorTex"), 0);
	glUniformMatrix4fv(gBufferShader("mvp"), 1, GL_FALSE, glm::value_ptr(mvp));
	glUniformMatrix4fv(gBufferShader("mv"), 1, GL_FALSE, glm::value_ptr(mv));
	glUniformMatrix3fv(gBufferShader("mn"), 1, GL_FALSE, glm::value_ptr(mn));
	mesh->render();
	gBufferShader.UnUse();
	gBuffer->unbind();
	*/
	
	////////////////////////////////////////////////////
	// SHADOW MAP
	////////////////////////////////////////////////////
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1, 1);

	glBindFramebuffer(GL_FRAMEBUFFER, ShadowMapManager->getFboId());
	shadowMap.Use();
	glViewport(0, 0, SHADOWMAPSIZE, SHADOWMAPSIZE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	light->computeMatrixes();
	glUniformMatrix4fv(shadowMap("mvp"), 1, GL_FALSE, glm::value_ptr(mvp_light));
	mesh->render();
	shadowMap.UnUse();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDisable(GL_POLYGON_OFFSET_FILL);
	//glDisable(GL_CULL_FACE);

	////////////////////////////////////////////////////
	// RSM
	////////////////////////////////////////////////////
	glBindFramebuffer(GL_FRAMEBUFFER, RSMFboManager->getFboId());
	rsmShader.Use();
	glViewport(0, 0, RSMSIZE, RSMSIZE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	light->computeMatrixes();
	glUniformMatrix4fv(rsmShader("mvp"), 1, GL_FALSE, glm::value_ptr(mvp_light));
	//glUniformMatrix4fv(rsmShader("mv"), 1, GL_FALSE, glm::value_ptr(v_light));
	glUniformMatrix4fv(rsmShader("m"), 1, GL_FALSE, glm::value_ptr(m));
	glUniform3f(rsmShader("v_lightPos"), lightPosition.x, lightPosition.y, lightPosition.z);
	//glUniformMatrix3fv(rsmShader("mn"), 1, GL_FALSE, glm::value_ptr(mn_light));
	mesh->render();
	rsmShader.UnUse();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	

	////////////////////////////////////////////////////
	// LIGHT INJECT
	////////////////////////////////////////////////////
	//!!!!!!TEMPORARY!!!!!!!!
	texManager.clear3Dtexture(texManager["LPVGridR"]);
	texManager.clear3Dtexture(texManager["LPVGridG"]);
	texManager.clear3Dtexture(texManager["LPVGridB"]);

	glViewport(0, 0, volumeDimensions.x, volumeDimensions.y); //!! Set vieport to width and height of 3D texture!!
	glDisable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (b_useLayeredFill) {
		glBindFramebuffer(GL_FRAMEBUFFER, lightInjectFBO->getFboId());
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		//Additive
		glBlendEquation(GL_FUNC_ADD);
		injectLight_layered.Use();

		glUniform1i(injectLight_layered("rsm_world_space_coords_tex"), 0);
		glUniform1i(injectLight_layered("rsm_normal_tex"), 1);
		glUniform1i(injectLight_layered("rsm_flux_tex"), 2);
		glUniform1i(injectLight_layered("i_RSMsize"), RSMSIZE);
		glUniform1f(injectLight_layered("f_cellSize"), cellSize);
		glUniform3f(injectLight_layered("v_gridDim"), volumeDimensions.x, volumeDimensions.y, volumeDimensions.z);
		glUniform3f(injectLight_layered("v_min"), vMin.x, vMin.y, vMin.z);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texManager["rsm_world_space_coords_tex"]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texManager["rsm_normal_tex"]);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, texManager["rsm_flux_tex"]);

		glBindVertexArray(VPLsVAO);//aktivujeme VAO
		glDrawArrays(GL_POINTS, 0, VPL_COUNT);
		glBindVertexArray(0);//deaktivujeme VAO
		injectLight_layered.UnUse();
		glDisable(GL_BLEND);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	} else {
		injectLight.Use();
		//texManager.clear3Dtexture(texManager["LPVGridR"]);
		//texManager.clear3Dtexture(texManager["LPVGridG"]);
		//texManager.clear3Dtexture(texManager["LPVGridB"]);

		glUniform1i(injectLight("LPVGridR"), 0);
		glUniform1i(injectLight("LPVGridG"), 1);
		glUniform1i(injectLight("LPVGridB"), 2);
		glUniform1i(injectLight("rsm_world_space_coords_tex"), 0);
		glUniform1i(injectLight("rsm_normal_tex"), 1);
		glUniform1i(injectLight("rsm_flux_tex"), 2);
		glUniform1i(injectLight("i_RSMsize"), RSMSIZE);
		glUniform1f(injectLight("f_cellSize"), cellSize);
		glUniform3f(injectLight("v_gridDim"), volumeDimensions.x, volumeDimensions.y, volumeDimensions.z);
		glUniform3f(injectLight("v_min"), vMin.x, vMin.y, vMin.z);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texManager["rsm_world_space_coords_tex"]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texManager["rsm_normal_tex"]);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, texManager["rsm_flux_tex"]);
		glBindImageTexture(0, texManager["LPVGridR"], 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA16F);
		glBindImageTexture(1, texManager["LPVGridG"], 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA16F);
		glBindImageTexture(2, texManager["LPVGridB"], 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA16F);
		glBindVertexArray(VPLsVAO);
		glDrawArrays(GL_POINTS, 0, VPL_COUNT);
		glBindVertexArray(0);
		injectLight.UnUse();
	}

	////////////////////////////////////////////////////
	// GEOMETRY INJECT
	////////////////////////////////////////////////////
	glViewport(0, 0, volumeDimensions.x, volumeDimensions.y); //!! Set vieport to width and height of 3D texture!!
	glDisable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	texManager.clear3Dtexture(texManager["GeometryVolume"]);
	if (b_useLayeredFill) {
		glBindFramebuffer(GL_FRAMEBUFFER, geometryInjectFBO->getFboId());
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		//Additive
		glBlendEquation(GL_FUNC_ADD);
		geometryInject_layered.Use();
		glUniform1i(geometryInject_layered("rsm_world_space_coords_tex"), 0);
		glUniform1i(geometryInject_layered("rsm_normal_tex"), 1);
		glUniform1i(geometryInject_layered("i_RSMsize"), RSMSIZE);
		glUniform1f(geometryInject_layered("f_cellSize"), cellSize);
		glUniform1f(geometryInject_layered("f_tanFovXHalf"), f_tanFovXHalf);
		glUniform1f(geometryInject_layered("f_tanFovYHalf"), f_tanFovYHalf);
		glUniform1f(geometryInject_layered("f_texelAreaModifier"), f_texelAreaModifier);
		glUniform3f(geometryInject_layered("v_gridDim"), volumeDimensions.x, volumeDimensions.y, volumeDimensions.z);
		glUniform3f(geometryInject_layered("v_min"), vMin.x, vMin.y, vMin.z);
		glUniform3f(geometryInject_layered("v_lightPos"), lightPosition.x, lightPosition.y, lightPosition.z);
		glUniformMatrix4fv(geometryInject_layered("m_lightView"), 1, GL_FALSE, glm::value_ptr(v_light));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texManager["rsm_world_space_coords_tex"]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texManager["rsm_normal_tex"]);
		glBindVertexArray(VPLsVAO);
		glDrawArrays(GL_POINTS, 0, VPL_COUNT);
		glBindVertexArray(0);
		geometryInject_layered.UnUse();
		glDisable(GL_BLEND);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	} else {
		geometryInject.Use();
		glUniform1i(geometryInject("GeometryVolume"), 0);
		glUniform1i(geometryInject("rsm_world_space_coords_tex"), 0);
		glUniform1i(geometryInject("rsm_normal_tex"), 1);
		glUniform1i(geometryInject("i_RSMsize"), RSMSIZE);
		glUniform1f(geometryInject("f_cellSize"), cellSize);
		glUniform1f(geometryInject("f_tanFovXHalf"), f_tanFovXHalf);
		glUniform1f(geometryInject("f_tanFovYHalf"), f_tanFovYHalf);
		glUniform1f(geometryInject("f_texelAreaModifier"), f_texelAreaModifier);
		glUniform3f(geometryInject("v_gridDim"), volumeDimensions.x, volumeDimensions.y, volumeDimensions.z);
		glUniform3f(geometryInject("v_min"), vMin.x, vMin.y, vMin.z);
		glUniform3f(geometryInject("v_lightPos"), lightPosition.x, lightPosition.y, lightPosition.z);
		glUniformMatrix4fv(geometryInject("m_lightView"), 1, GL_FALSE, glm::value_ptr(v_light));
		//glUniformMatrix4fv(geometryInject("m_lightView"), 1, GL_FALSE, glm::value_ptr(v));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texManager["rsm_world_space_coords_tex"]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texManager["rsm_normal_tex"]);
		glBindImageTexture(0, texManager["GeometryVolume"], 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA16F);
		glBindVertexArray(VPLsVAO);
		glDrawArrays(GL_POINTS, 0, VPL_COUNT);
		glBindVertexArray(0);
		geometryInject.UnUse();
	}

	//float data[5 * 5 * 5 * 4];
	//for (unsigned i = 0; i<5 * 5 * 5 * 4; ++i)data[i] = 0.;
	//glBindTexture(GL_TEXTURE_3D, texManager["LPVGridR"]);
	//glGetTexImage(GL_TEXTURE_3D, 0, GL_RGBA, GL_FLOAT, data);
	//std::cerr << data[0]  <<" " << data[4*5] << std::endl;

	
	////////////////////////////////////////////////////
	// LIGHT PROPAGATION
	////////////////////////////////////////////////////
	glViewport(0, 0, volumeDimensions.x, volumeDimensions.y); //!! Set vieport to width and height of 3D texture!!
	glDisable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	propagationShader.Use();
	b_firstPropStep = true;

	//GLfloat data[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	//glClearTexImage(texManager["AccumulatorLPV"], 0, GL_RGBA, GL_FLOAT, &data[0]);
	//texManager.clear3Dtexture(texManager["AccumulatorLPV"]);
	texManager.clear3Dtexture(texManager["RAccumulatorLPV"]);
	texManager.clear3Dtexture(texManager["GAccumulatorLPV"]);
	texManager.clear3Dtexture(texManager["BAccumulatorLPV"]);
	//texManager.clear3Dtexture(propTextures[1]);

	glUniform1i(propagationShader("RAccumulatorLPV"), 0);
	glUniform1i(propagationShader("GAccumulatorLPV"), 1);
	glUniform1i(propagationShader("BAccumulatorLPV"), 2);

	glUniform1i(propagationShader("RLightGridForNextStep"), 3);
	glUniform1i(propagationShader("GLightGridForNextStep"), 4);
	glUniform1i(propagationShader("BLightGridForNextStep"), 5);

	//glUniform1i(propagationShader("LightGrid"), 3);
	//glUniform1i(propagationShader("LightGridForNextStep"), 4);

	//glUniform1i(propagationShader("GeometryVolume"), 5);
	glUniform1i(propagationShader("GeometryVolume"), 0);
	glUniform1i(propagationShader("LPVGridR"), 1);
	glUniform1i(propagationShader("LPVGridG"), 2);
	glUniform1i(propagationShader("LPVGridB"), 3);
	//glUniform1i(propagationShader("b_firstPropStep"), b_firstPropStep);
	glUniform3f(propagationShader("v_gridDim"), volumeDimensions.x, volumeDimensions.y, volumeDimensions.z);

	//glBindImageTexture(0, texManager["AccumulatorLPV"], 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA16F);
	glBindImageTexture(0, texManager["RAccumulatorLPV"], 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA16F);
	glBindImageTexture(1, texManager["GAccumulatorLPV"], 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA16F);
	glBindImageTexture(2, texManager["BAccumulatorLPV"], 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA16F);
	//glBindImageTexture(5, texManager["GeometryVolume"], 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA16F);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, texManager["GeometryVolume"]);

	if (useMultiStepPropagation) {
		for (int i = 1; i < PROPAGATION_STEPS; i++) {
			//glUniform1i(propagationShader("AccumulatorLPV"), 0);
			if (i > 0)
				b_firstPropStep = false;
			glUniform1i(propagationShader("b_firstPropStep"), b_firstPropStep);
			glUniform1i(propagationShader("b_useOcclusion"), b_useOcclusion);
			//glBindImageTexture(3, propTextures[i-1], 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA16F);
			//glBindImageTexture(4, propTextures[i], 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA16F);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_3D, propTextures[i - 1].red);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_3D, propTextures[i - 1].green);
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_3D, propTextures[i - 1].blue);

			glBindImageTexture(3, propTextures[i].red, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA16F);
			glBindImageTexture(4, propTextures[i].green, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA16F);
			glBindImageTexture(5, propTextures[i].blue, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA16F);

			glBindVertexArray(PropagationVAO);
			glDrawArrays(GL_POINTS, 0, volumeDimensionsMult);
			glBindVertexArray(0);
		}

		for (int j = 1; j < PROPAGATION_STEPS; j++) {
			texManager.clear3Dtexture(propTextures[j].red);
			texManager.clear3Dtexture(propTextures[j].green);
			texManager.clear3Dtexture(propTextures[j].blue);
		}
	}
	else {
		//glBindImageTexture(3, propTextures[0], 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA16F);
		//glBindImageTexture(4, propTextures[1], 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA16F);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_3D, propTextures[0].red);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_3D, propTextures[0].green);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_3D, propTextures[0].blue);

		glBindImageTexture(3, propTextures[1].red, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA16F);
		glBindImageTexture(4, propTextures[1].green, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA16F);
		glBindImageTexture(5, propTextures[1].blue, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA16F);

		glUniform1i(propagationShader("b_firstPropStep"), b_firstPropStep);

		glBindVertexArray(PropagationVAO);
		glDrawArrays(GL_POINTS, 0, volumeDimensionsMult);
		glBindVertexArray(0);

		texManager.clear3Dtexture(propTextures[1].red);
		texManager.clear3Dtexture(propTextures[1].green);
		texManager.clear3Dtexture(propTextures[1].blue);
	}
	test = false;
	propagationShader.UnUse();

	////////////////////////////////////////////////////
	// RENDER SCENE TO TEXTURE
	////////////////////////////////////////////////////
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	//glCullFace(GL_BACK);
	glViewport(0, 0, WIDTH, HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, fboManager->getFboId());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	basicShader.Use();

#ifdef USESAMPLER3D
	//glUniform1i(basicShader("AccumulatorLPV"), 3);
	//glActiveTexture(GL_TEXTURE3);
	//glBindTexture(GL_TEXTURE_3D, texManager["AccumulatorLPV"]);
	glUniform1i(basicShader("RAccumulatorLPV"), 3);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_3D, texManager["RAccumulatorLPV"]);
	glUniform1i(basicShader("GAccumulatorLPV"), 4);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_3D, texManager["GAccumulatorLPV"]);
	glUniform1i(basicShader("BAccumulatorLPV"), 5);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_3D, texManager["BAccumulatorLPV"]);
#else
	//glUniform1i(basicShader("AccumulatorLPV"), 0);
	//glBindImageTexture(0, texManager["AccumulatorLPV"], 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA16F);
	glUniform1i(basicShader("RAccumulatorLPV"), 0);
	glBindImageTexture(0, texManager["RAccumulatorLPV"], 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA16F);
	glUniform1i(basicShader("GAccumulatorLPV"), 1);
	glBindImageTexture(1, texManager["GAccumulatorLPV"], 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA16F);
	glUniform1i(basicShader("BAccumulatorLPV"), 2);
	glBindImageTexture(2, texManager["BccumulatorLPV"], 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA16F);
#endif
	
	glUniform1f(basicShader("f_cellSize"), cellSize);
	glUniform1f(basicShader("f_indirectAttenuation"), f_indirectAttenuation);// f_indirectAttenuation
	glUniform3f(basicShader("v_gridDim"), volumeDimensions.x, volumeDimensions.y, volumeDimensions.z);
	glUniform3f(basicShader("v_min"), vMin.x, vMin.y, vMin.z);
	//glBindImageTexture(0, texManager["AccumulatorLPV"], 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA16F);
	//glUniform1i(basicShader("tex"), 0); //Texture unit 0 is for base images.
	glUniform1i(basicShader("depthTexture"), 1); //Texture unit 1 is for shadow maps.
	glUniformMatrix4fv(basicShader("mvp"), 1, GL_FALSE, glm::value_ptr(mvp));
	glUniformMatrix4fv(basicShader("mv"), 1, GL_FALSE, glm::value_ptr(mv));
	glUniformMatrix4fv(basicShader("v"), 1, GL_FALSE, glm::value_ptr(v));
	glUniformMatrix4fv(basicShader("shadowMatrix"), 1, GL_FALSE, glm::value_ptr(biasMatrix*mvp_light));
	glUniformMatrix3fv(basicShader("mn"), 1, GL_FALSE, glm::value_ptr(mn));
	glUniform3f(basicShader("vLightPos"), lightPosition.x, lightPosition.y, lightPosition.z);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texManager["rsm_depth_tex"]);
	mesh->render();
	glBindTexture(GL_TEXTURE_2D, 0);
	basicShader.UnUse();

	dd->setVPMatrix(mvp);
	dd->draw();
	////////////////////////////////////////////////////
	// VPL DEBUG DRAW
	////////////////////////////////////////////////////
#ifdef VPL_DEBUG
	glEnable(GL_PROGRAM_POINT_SIZE);
	glPointSize(2.5f);
	VPLsDebug.Use();
	glUniformMatrix4fv(VPLsDebug("mvp"), 1, GL_FALSE, glm::value_ptr(mvp));
	glUniform1i(VPLsDebug("i_RSMsize"), RSMSIZE);
	glUniform1i(VPLsDebug("rsm_world_space_coords_tex"), 0);
	glUniform1i(VPLsDebug("rsm_normal_tex"), 1);
	glUniform1i(VPLsDebug("b_useNormalOffset"), b_useNormalOffset);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texManager["rsm_world_space_coords_tex"]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texManager["rsm_normal_tex"]);
	glBindVertexArray(VPLsVAO);
	glDrawArrays(GL_POINTS, 0, VPL_COUNT);
	glBindVertexArray(0);
	VPLsDebug.UnUse();
	glDisable(GL_PROGRAM_POINT_SIZE);
#endif

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	////////////////////////////////////////////////////
	// FINAL COMPOSITION
	////////////////////////////////////////////////////
	//Draw quad on screen
	glViewport(0, 0, WIDTH, HEIGHT);
	glDisable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ctv2->setTexture(texManager["render_tex"]);
	ctv2->draw();
	
}

void DisplayTexture(CTextureViewer * ctv) {

	//glEnable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	ctv->draw();
}
void Finalize(void) {

	delete ctv;
	delete ctv2;

	delete controlCamera;
	delete mesh;
	delete fboManager;
	delete RSMFboManager;
	delete light;
	delete dd;
	delete gBuffer;
	delete propagationFBO;
	delete geometryInjectFBO;
	delete lightInjectFBO;
}
void Reshape(int width, int height){
	glViewport(0, 0, width, height);
	aspect = float(height) / float(width);
}

int main() {
	//for (int i = 0; i < 25; i++) {
	//	std::cout << i << ":\t" << i % 5 << ", " << i / 5 << std::endl;
	//}

	//ilutRenderer(ILUT_OPENGL);
	ilInit();
	iluInit();
	//ilutInit();


	SDL_Window *mainwindow; /* Our window handle */
	SDL_GLContext maincontext; /* Our opengl context handle */
	SDL_Window *w2;
	SDL_GLContext c2;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) { /* Initialize SDL's Video subsystem */
		std::cout << "Unable to initialize SDL";
		return 1;
	}

	/* Request opengl 4.4 context. */
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	/* Turn on double buffering with a 24bit Z buffer.
	* You may need to change this to 16 or 32 for your system */
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	//SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);

	/* Create our window centered at 512x512 resolution */
	mainwindow = SDL_CreateWindow("Window title goes here", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		WIDTH, HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (!mainwindow){ /* Die if creation failed */
		std::cout << "SDL Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}

	/* Create our opengl context and attach it to our window */
	maincontext = SDL_GL_CreateContext(mainwindow);
	//SDL_GL_MakeCurrent(mainwindow, maincontext);
#ifdef W2
	w2 = SDL_CreateWindow("Window title goes here #2", 50, 50,
		WIDTH, HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (!w2){ // Die if creation failed 
		std::cout << "SDL Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}
#endif
#ifdef CTV
	SDL_Window *w3;
	w3 = SDL_CreateWindow("Window title goes here #3", 50, 50,
		WIDTH, HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (!w3){ /* Die if creation failed */
		std::cout << "SDL Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}

	SDL_Window *w4;
	w4 = SDL_CreateWindow("Window title goes here #4", 50, 50,
		WIDTH, HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (!w4){ /* Die if creation failed */
		std::cout << "SDL Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}

	SDL_Window *w5;
	w5 = SDL_CreateWindow("Window title goes here #4", 50, 50,
		WIDTH, HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (!w5){ /* Die if creation failed */
		std::cout << "SDL Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}
#endif

	GLenum rev;
	glewExperimental = GL_TRUE;
	rev = glewInit();

	if (GLEW_OK != rev){
		std::cout << "Error: " << glewGetErrorString(rev) << std::endl;
		exit(1);
	}
	else {
		std::cout << "GLEW Init: Success!" << std::endl;
		std::cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;
	}



	/* This makes our buffer swap syncronized with the monitor's vertical refresh */

	SDL_GL_SetSwapInterval(1);

	bool quit = false;

	/*std::cout << "OpenGL 2.0: " << ((GLEW_VERSION_2_0 != 0) ? "Available" : "Unavailable") << std::endl;
	std::cout << "OpenGL 2.1: " << ((GLEW_VERSION_2_1 != 0) ? "Available" : "Unavailable") << std::endl;

	std::cout << "OpenGL 3.0: " << ((GLEW_VERSION_3_0 != 0) ? "Available" : "Unavailable") << std::endl;
	std::cout << "OpenGL 3.1: " << ((GLEW_VERSION_3_1 != 0) ? "Available" : "Unavailable") << std::endl;
	std::cout << "OpenGL 3.2: " << ((GLEW_VERSION_3_2 != 0) ? "Available" : "Unavailable") << std::endl;
	std::cout << "OpenGL 3.3: " << ((GLEW_VERSION_3_3 != 0) ? "Available" : "Unavailable") << std::endl;

	std::cout << "OpenGL 4.0: " << ((GLEW_VERSION_4_0 != 0) ? "Available" : "Unavailable") << std::endl;
	std::cout << "OpenGL 4.1: " << ((GLEW_VERSION_4_1 != 0) ? "Available" : "Unavailable") << std::endl;
	std::cout << "OpenGL 4.2: " << ((GLEW_VERSION_4_3 != 0) ? "Available" : "Unavailable") << std::endl;
	std::cout << "OpenGL 4.3: " << ((GLEW_VERSION_4_3 != 0) ? "Available" : "Unavailable") << std::endl;
	std::cout << "OpenGL 4.4: " << ((GLEW_VERSION_4_4 != 0) ? "Available" : "Unavailable") << std::endl;
	//std::cout << "OpenGL 4.5: " << ((GLEW_VERSION_4_5 != 0) ? "Available" : "Unavailable") << std::endl;*/

	Initialize(mainwindow);
	Reshape(WIDTH, HEIGHT);

	SDL_Event event;
	const Uint8 * keys;
	Uint32 old_time, current_time;
	current_time = SDL_GetTicks();
	while (!quit){
		while (SDL_PollEvent(&event)){
			if (event.type == SDL_QUIT){
				//std::cout << "yes" << std::endl;
				quit = true;
			}
#ifdef W2
			if (event.type == SDL_WINDOWEVENT) {
				switch (event.window.event) {
				case SDL_WINDOWEVENT_CLOSE:
					//SDL_Log("Window %d closed", event.window.windowID);
					SDL_DestroyWindow((event.window.windowID > 1) ? w2 : mainwindow);
					break;
				}
			}
#endif
			if (event.type == SDL_MOUSEMOTION) {
				if (event.motion.state & SDL_BUTTON_LMASK)
				{
					controlCamera->moved = true;
					controlCamera->computeMatricesFromInputs();
					controlCamera->moved = false;
				}
			}

			if (event.type == SDL_KEYDOWN && event.key.repeat == 0) {
				if (event.key.keysym.sym == SDLK_p)
					useMultiStepPropagation = !useMultiStepPropagation;
				if (event.key.keysym.sym == SDLK_o)
					b_useOcclusion = !b_useOcclusion;
				if (event.key.keysym.sym == SDLK_c) {
					f_indirectAttenuation += 0.1;
				}
				if (event.key.keysym.sym == SDLK_v) {
					if (f_indirectAttenuation >= 0.2) {
						f_indirectAttenuation -= 0.1;
					}
				}
				if (event.key.keysym.sym == SDLK_l) {
					b_useLayeredFill = !b_useLayeredFill;
				}
			}
		}
		old_time = current_time;
		current_time = SDL_GetTicks();
		ftime = (current_time - old_time) / 1000.0f;
		//cout << ftime << endl;

		keys = SDL_GetKeyboardState(NULL);
		if (keys[SDL_SCANCODE_W]) {
			controlCamera->setPosition(controlCamera->getPosition() + (controlCamera->getDirection() * movementSpeed * ftime));
		}
		else if (keys[SDL_SCANCODE_S]) {
			controlCamera->setPosition(controlCamera->getPosition() - (controlCamera->getDirection() * movementSpeed * ftime));
		}
		else if (keys[SDL_SCANCODE_A]) {
			controlCamera->setPosition(controlCamera->getPosition() - (controlCamera->getRight() * movementSpeed * ftime));
		}
		else if (keys[SDL_SCANCODE_D]) {
			controlCamera->setPosition(controlCamera->getPosition() + (controlCamera->getRight() * movementSpeed * ftime));
		}
		else if (keys[SDL_SCANCODE_KP_8]) {
			light->setPosition(light->getPosition() + (glm::vec3(0, 1, 0)* movementSpeed * 2.0f * ftime));
		}
		else if (keys[SDL_SCANCODE_KP_2]) {
			light->setPosition(light->getPosition() - (glm::vec3(0, 1, 0)* movementSpeed * 2.0f * ftime));
		}
		else if (keys[SDL_SCANCODE_KP_4]) {
			light->setPosition(light->getPosition() - (glm::vec3(1, 0, 0)* movementSpeed * 2.f *ftime));
		}
		else if (keys[SDL_SCANCODE_KP_6]) {
			light->setPosition(light->getPosition() + (glm::vec3(1, 0, 0)* movementSpeed * 2.f* ftime));
		}
		//else if (keys[SDL_SCANCODE_KP_9]) {
		//	light->setPosition(light->getPosition() - (glm::vec3(0, 0, 1)* movementSpeed * ftime));
		//}
		//else if (keys[SDL_SCANCODE_KP_3]) {
		//	light->setPosition(light->getPosition() + (glm::vec3(0, 0, 1)* movementSpeed * ftime));
		//}
		else if (keys[SDL_SCANCODE_KP_MINUS]) {
			light->setVerAngle(light->getVerAngle() - 0.01f);
		}
		else if (keys[SDL_SCANCODE_KP_PLUS]) {
			light->setVerAngle(light->getVerAngle() + 0.01f);
		}
		else if (keys[SDL_SCANCODE_KP_DIVIDE]) {
			light->setHorAngle(light->getHorAngle() + 0.01f);
		}
		else if (keys[SDL_SCANCODE_KP_MULTIPLY]) {
			light->setHorAngle(light->getHorAngle() - 0.01f);
		}

#ifdef CTV

		SDL_GL_MakeCurrent(w3, maincontext);
		/*ctv->setTexture(texManager["rsm_depth_tex"]);*/
		//rsm_world_space_coords_tex
		//rsm_normal_tex
		//rsm_flux_tex
		ctv->setTexture(texManager["rsm_normal_tex"]);
		//ctv->setDepthOnly(true);
		DisplayTexture(ctv);
		SDL_GL_SwapWindow(w3);

		SDL_GL_MakeCurrent(w4, maincontext);
		/*ctv->setTexture(texManager["rsm_depth_tex"]);*/
		//rsm_world_space_coords_tex
		//rsm_normal_tex
		//rsm_flux_tex
		ctv->setTexture(texManager["rsm_world_space_coords_tex"]);
		//ctv->setDepthOnly(true);
		DisplayTexture(ctv);
		SDL_GL_SwapWindow(w4);
#endif
#ifdef W2
		SDL_GL_MakeCurrent(w2, maincontext);
		//ctv->setTexture(texManager["rsm_depth_tex"]);
		//rsm_world_space_coords_tex
		//rsm_normal_tex
		//rsm_flux_tex
		ctv->setTexture(texManager["rsm_flux_tex"]);
		//ctv->setDepthOnly(true);
		DisplayTexture(ctv);
		SDL_GL_SwapWindow(w2);
#endif
		SDL_GL_MakeCurrent(mainwindow, maincontext);
		Display();
		SDL_GL_SwapWindow(mainwindow);

	}
	//std::cout << "MAX: " << mesh->getBoundingBox()->getMax().x << "," << mesh->getBoundingBox()->getMax().y << "," << mesh->getBoundingBox()->getMax().z << std::endl;
	//std::cout << "MIN: " << mesh->getBoundingBox()->getMin().x << "," << mesh->getBoundingBox()->getMin().y << "," << mesh->getBoundingBox()->getMin().z << std::endl;
	//std::vector<glm::vec3> points = mesh->getBoundingBox()->getPoints();
	//for (std::vector<glm::vec3>::iterator it = points.begin(); it != points.end(); ++it) {
	//	std::cout << it - points.begin() + 1<< ": " << (*it).x << ", " << (*it).y << ", " << (*it).z << std::endl;
	//}

	//std::vector<glm::vec2> uv;
	//uv.push_back(glm::vec2(1.0));



	//Finalize();

	/* Delete our opengl context, destroy our window, and shutdown SDL */
	SDL_GL_DeleteContext(maincontext);
	SDL_DestroyWindow(mainwindow);
#ifdef W2
	SDL_DestroyWindow(w2);
#endif

	SDL_Quit();

	return 0;
}

