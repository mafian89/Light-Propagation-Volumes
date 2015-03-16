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
GLSLShader basicShader, depthShader, shadowMap, injectLight;
Mesh * mesh;
float movementSpeed = 10.0f;
float ftime;
GLuint tex;
//glm::vec3 lightPosition(0.0, 4.0, 2.0);
CTextureManager texManager;
CFboManager * fboManager = new CFboManager();
CFboManager * RSMFboManager = new CFboManager();
CFboManager * ShadowMapManager = new CFboManager();
CFboManager * testInject = new CFboManager();
CLightObject * light;
DebugDrawer * dd;
GLuint depthPassFBO;
GLint texture_units, max_color_attachments;

glm::mat4 biasMatrix(
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 0.5, 0.0,
	0.5, 0.5, 0.5, 1.0
	);

//#define CTV

#ifndef NOTEST
GLuint emptyVAO, tmpVBO;
#define TEST_NUM_POINT 1
#endif



void Initialize(SDL_Window * w) {
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &texture_units);
	//glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &max_color_attachments);
	//std::cout << "Max color attachments: " << max_color_attachments << std::endl;
	//tex = loadImage("../textures/texture.png");
	/*
	Light POSITION vector: (4.81105, 18.9061, 28.3497)
	Light DIRECTION vector: (0.0023295, -0.439015, -0.898477)
	Light horizotnal angle: 3.139
	Light vertical angle: -0.454502

	Light POSITION vector: (7.00241, 48.0456, 10.0773)
	Light DIRECTION vector: (0.00145602, -0.827421, -0.56158)
	Light horizotnal angle: 3.139
	Light vertical angle: -0.9745

	Light POSITION vector: (17.34, 2.04, 0)
	Light DIRECTION vector: (-0.972737, 0.198669, -0.119638)
	Light horizotnal angle: 4.59001
	Light vertical angle: 0.2
	*/

#ifdef DEBUGLIGHT
	light = new CLightObject(glm::vec3(17.34, 2.04, 0), glm::vec3(-0.972737, 0.198669, -0.119638));
	light->setHorAngle(4.59001);
	light->setVerAngle(0.2);
#else
	light = new CLightObject(glm::vec3(7.00241, 48.0456, 10.0773), glm::vec3(0.00145602, -0.827421, -0.56158));
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

	depthShader.LoadFromFile(GL_VERTEX_SHADER, std::string("../shaders/RSMpass.vs").c_str());
	depthShader.LoadFromFile(GL_FRAGMENT_SHADER, std::string("../shaders/RSMpass.frag").c_str());
	depthShader.CreateAndLinkProgram();

	shadowMap.LoadFromFile(GL_VERTEX_SHADER, std::string("../shaders/depthOnly.vs").c_str());
	shadowMap.LoadFromFile(GL_FRAGMENT_SHADER, std::string("../shaders/depthOnly.frag").c_str());
	shadowMap.CreateAndLinkProgram();

	injectLight.LoadFromFile(GL_VERTEX_SHADER, std::string("../shaders/lightInject.vs").c_str());
	injectLight.LoadFromFile(GL_GEOMETRY_SHADER, std::string("../shaders/lightInject.gs").c_str());
	injectLight.LoadFromFile(GL_FRAGMENT_SHADER, std::string("../shaders/lightInject.frag").c_str());
	injectLight.CreateAndLinkProgram();

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
	*/
	controlCamera->initControlCamera(glm::vec3(11.7542, 14.1148, 0.822185), w, 4.25502, -0.508, WIDTH, HEIGHT, 1.0, 1000.0);

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
	basicShader.UnUse();

	depthShader.Use();
	depthShader.AddUniform("mvp");
	depthShader.AddUniform("m");
	depthShader.AddUniform("mn");
	depthShader.UnUse();

	shadowMap.Use();
	shadowMap.AddUniform("mvp");
	shadowMap.UnUse();

	injectLight.Use();
	injectLight.AddUniform("LPVGridR");
	injectLight.AddUniform("LPVGridG");
	injectLight.AddUniform("LPVGridB");
	injectLight.UnUse();

	////////////////////////////////////////////////////
	// TEST STUFF
	////////////////////////////////////////////////////
#ifndef NOTEST
	injectLight.Use();
	//Generate VAO
	glGenVertexArrays(1, &emptyVAO);
	
	//Bind VAO
	glBindVertexArray(emptyVAO);

	//Generate VBO
	glGenBuffers(1, &tmpVBO);
	//Bind VBO
	glBindBuffer(GL_ARRAY_BUFFER, tmpVBO);



	float testPoints[2*TEST_NUM_POINT];
	float step = 1.0 / TEST_NUM_POINT;
	for (int i = 0; i < TEST_NUM_POINT; ++i) {
		//testPoints[i * 2] = -0.5f + i*step;
		//testPoints[i * 2 + 1] = 0.0f;
		testPoints[i * 2] = 0.0f;
		testPoints[i * 2 + 1] = 0.0f;
	}

	//std::cout << sizeof(testPoints) << std::endl;
	//std::cout << 2 * TEST_NUM_POINT * sizeof(float) << std::endl;

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
#endif

	////////////////////////////////////////////////////
	// LOAD MODELS
	////////////////////////////////////////////////////
	mesh = new Mesh("../models/sponza.obj");
	dd = new DebugDrawer(GL_LINE_STRIP, &(mesh->getBoundingBox()->getDebugDrawPoints()), NULL, NULL);
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
	texManager.createRGBA16F3DTexture("LPVGridR", 5, 5, 5, GL_NEAREST, GL_CLAMP_TO_EDGE);
	texManager.createRGBA16F3DTexture("LPVGridG", 5, 5, 5, GL_NEAREST, GL_CLAMP_TO_EDGE);
	texManager.createRGBA16F3DTexture("LPVGridB", 5, 5, 5, GL_NEAREST, GL_CLAMP_TO_EDGE);
	//texManager.createRGBA3DTexture("test3D", 5, 5, 5, GL_NEAREST, GL_CLAMP_TO_EDGE);
	texManager.createRGBA16F3DTexture("test3D", 5, 5, 5, GL_NEAREST, GL_CLAMP_TO_EDGE);
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

	testInject->initFbo();
	//testInject->genRenderDepthBuffer(WIDTH, HEIGHT);
	//testInject->bindRenderDepthBuffer();
	testInject->bind3DTextureToFbo(GL_COLOR_ATTACHMENT0, texManager["test3D"]);
	testInject->setDrawBuffers();
	if (!testInject->checkFboStatus()) {
		return;
	}

	//glGenFramebuffers(1, &depthPassFBO);
	//glBindFramebuffer(GL_FRAMEBUFFER, depthPassFBO);

	//// Depth texture. Slower than a depth buffer, but you can sample it later in your shader
	//glBindTexture(GL_TEXTURE_2D, texManager["rsm_depth_tex"]);

	//glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texManager["rsm_depth_tex"], 0);

	//glDrawBuffer(GL_NONE); // No color buffer is drawn to.

	//// Always check that our framebuffer is ok
	//if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	//	return;
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//IN CASE OF PROBLEMS UNCOMMENT LINE BELOW
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
float rot = 0.0;
float elevation = 0.0;
float rotSpeed = 30.0f;
float elevationSpeed = 1.0f;
bool drawn = false;
void Display() {
	//Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Clear color
	glClearColor(0.0, 0.0, 0.6, 1.0);
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

	glm::mat4 mvp_light = light->getProjMatrix() * light->getViewMatrix() * m;
	//glm::mat3 mn_light = glm::transpose(glm::inverse(glm::mat3(m)));
	
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

	glBindFramebuffer(GL_FRAMEBUFFER, RSMFboManager->getFboId());
	depthShader.Use();
	glViewport(0, 0, RSMSIZE, RSMSIZE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	light->computeMatrixes();
	glUniformMatrix4fv(depthShader("mvp"), 1, GL_FALSE, glm::value_ptr(mvp_light));
	glUniformMatrix4fv(depthShader("m"), 1, GL_FALSE, glm::value_ptr(m));
	//glUniformMatrix3fv(basicShader("mn"), 1, GL_FALSE, glm::value_ptr(mn_light));
	mesh->render();
	depthShader.UnUse();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);




	glDisable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	glViewport(0, 0, WIDTH, HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, fboManager->getFboId());
	basicShader.Use();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glUniform1i(basicShader("tex"), 0); //Texture unit 0 is for base images.
	glUniform1i(basicShader("depthTexture"), 1); //Texture unit 1 is for shadow maps.
	glUniformMatrix4fv(basicShader("mvp"), 1, GL_FALSE, glm::value_ptr(mvp));
	glUniformMatrix4fv(basicShader("mv"), 1, GL_FALSE, glm::value_ptr(mv));
	glUniformMatrix4fv(basicShader("v"), 1, GL_FALSE, glm::value_ptr(v));
	glUniformMatrix4fv(basicShader("shadowMatrix"), 1, GL_FALSE, glm::value_ptr(biasMatrix*mvp_light));
	glUniformMatrix3fv(basicShader("mn"), 1, GL_FALSE, glm::value_ptr(mn));
	glm::vec3 lightPosition = light->getPosition();
	glUniform3f(basicShader("vLightPos"), lightPosition.x, lightPosition.y, lightPosition.z);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texManager["rsm_depth_tex"]);
	mesh->render();
	glBindTexture(GL_TEXTURE_2D, 0);
	basicShader.UnUse();


	//glClampColorARB(GL_CLAMP_VERTEX_COLOR, GL_FALSE);
	//glClampColorARB(GL_CLAMP_FRAGMENT_COLOR, GL_FALSE);
	//glBindImageTexture(1, testImageTexture, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA16F);
	dd->setVPMatrix(mvp);
	dd->draw();
	//glClampColorARB(GL_CLAMP_VERTEX_COLOR, GL_TRUE);
	//glClampColorARB(GL_CLAMP_FRAGMENT_COLOR, GL_TRUE);

	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, testInject->getFboId());
	//glViewport(0, 0, WIDTH, HEIGHT);
	glViewport(0, 0, 5, 5); //!! Set vieport to width and height of 3D texture!!
	glDisable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	injectLight.Use();
	//glUniform1i(injectLight("LPVGridR"), 0);
	//glUniform1i(injectLight("LPVGridG"), 1);
	//glUniform1i(injectLight("LPVGridB"), 2);
	//glBindImageTexture(0, texManager["LPVGridR"], 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA16F);
	//glBindImageTexture(1, texManager["LPVGridG"], 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA16F);
	//glBindImageTexture(2, texManager["LPVGridB"], 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA16F);
	glBindVertexArray(emptyVAO);//aktivujeme VAO
	glDrawArrays(GL_POINTS, 0, TEST_NUM_POINT);
	glBindVertexArray(0);//deaktivujeme VAO
	injectLight.UnUse();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	
	//Draw quad on screen
	glViewport(0, 0, WIDTH, HEIGHT);
	glDisable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.6, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ctv2->setTexture(texManager["render_tex"]);
	ctv2->draw();
	

	
	//if (!drawn) {
	//	unsigned int size = 64 *27 * 39*sizeof(float);
	//	float *pixels = new float[size];
	//	glGetTextureImage(testImageTexture, 0, GL_RED, GL_FLOAT, 4*sizeof(float), &pixels[0]);
	//	drawn = true;
	//	for (int i = 0; i < (64 * 27 * 39); i++) {
	//		if (pixels[i] > 0.0)
	//			std::cout << pixels[i];
	//	}
	//}
}

void DisplayTexture(CTextureViewer * ctv) {

	//glEnable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
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
}
void Reshape(int width, int height){
	glViewport(0, 0, width, height);
	aspect = float(height) / float(width);
}

int main() {

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
	//SDL_DestroyWindow(w2);

	SDL_Quit();

	return 0;
}

