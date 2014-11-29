#include "common.h"
#include "CTextureViewer.h"


#ifdef _MSC_VER
#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib,"devil.lib")
#pragma comment(lib,"ILU.lib")
#pragma comment(lib,"ILUT.lib")
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
GLSLShader basicShader, depthShader;
Mesh * mesh;
Mesh * mesh2;
float movementSpeed = 4.0f;
float ftime;
GLuint tex;
//glm::vec3 lightPosition(0.0, 4.0, 2.0);
CTextureManager texManager;
CFboManager * fboManager = new CFboManager();
CFboManager * RSMFboManager = new CFboManager();
CLightObject * light;
GLuint depthPassFBO;
GLint texture_units;

glm::mat4 biasMatrix(
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 0.5, 0.0,
	0.5, 0.5, 0.5, 1.0
	);

#define CTV




void Initialize(SDL_Window * w) {
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &texture_units);

	//tex = loadImage("../textures/texture.png");
	/*
	Light POSITION vector: (4.81105, 18.9061, 28.3497)
	Light DIRECTION vector: (0.0023295, -0.439015, -0.898477)
	Light horizotnal angle: 3.139
	Light vertical angle: -0.454502
	*/
	//light = new CLightObject(glm::vec3(0.0, 20.0, 5.0), glm::vec3(0, 0, 0));
	light = new CLightObject(glm::vec3(4.81105, 18.9061, 28.34975), glm::vec3(0.0023295, -0.439015, -0.898477));
	light->setHorAngle(3.139);
	light->setVerAngle(-0.454502);
#ifdef CTV
	ctv = new CTextureViewer(0, "../shaders/textureViewer.vs", "../shaders/textureViewer.frag");
	ctv2 = new CTextureViewer(0, "../shaders/textureViewer.vs", "../shaders/textureViewer.frag");
#endif
	////////////////////////////////////////////////////
	// SHADERS INIT
	////////////////////////////////////////////////////
	basicShader.LoadFromFile(GL_VERTEX_SHADER, std::string("../shaders/basicShader.vs").c_str());
	basicShader.LoadFromFile(GL_FRAGMENT_SHADER, std::string("../shaders/basicShader.frag").c_str());
	basicShader.CreateAndLinkProgram();

	depthShader.LoadFromFile(GL_VERTEX_SHADER, std::string("../shaders/RSMpass.vs").c_str());
	depthShader.LoadFromFile(GL_FRAGMENT_SHADER, std::string("../shaders/RSMpass.frag").c_str());
	depthShader.CreateAndLinkProgram();
	////////////////////////////////////////////////////
	// CAMERA INIT
	////////////////////////////////////////////////////
	controlCamera->initControlCamera(glm::vec3(.0, .0, 5.0), w, 3.14, 0.0, 800, 600, 0.1, 1000.0);

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

	////////////////////////////////////////////////////
	// LOAD MODELS
	////////////////////////////////////////////////////
	mesh = new Mesh("../models/mix.obj");
	mesh2 = new Mesh("../models/plane.obj");

	////////////////////////////////////////////////////
	// TEXTURE INIT
	////////////////////////////////////////////////////
	texManager.createTexture("render_tex", "", WIDTH, HEIGHT, GL_NEAREST, GL_RGBA16F, GL_RGBA, false);
	texManager.createTexture("rsm_normal_tex", "", WIDTH, HEIGHT, GL_NEAREST, GL_RGBA16F, GL_RGBA, false);
	texManager.createTexture("rsm_world_space_coords_tex", "", WIDTH, HEIGHT, GL_NEAREST, GL_RGBA16F, GL_RGBA, false);
	texManager.createTexture("rsm_flux_tex", "", WIDTH, HEIGHT, GL_NEAREST, GL_RGBA16F, GL_RGBA, false);
	texManager.createTexture("rsm_depth_tex", "", SHADOWMAPSIZE, SHADOWMAPSIZE, GL_NEAREST, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, true);

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
	RSMFboManager->bindToFbo(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texManager["rsm_world_space_coords_tex"]);
	RSMFboManager->bindToFbo(GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, texManager["rsm_normal_tex"]);
	RSMFboManager->bindToFbo(GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, texManager["rsm_flux_tex"]);
	RSMFboManager->bindToFbo(GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texManager["rsm_depth_tex"]);
	RSMFboManager->setDrawBuffers();
	if (!RSMFboManager->checkFboStatus()){
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

}
float rot = 0.0;
float elevation = 0.0;
float rotSpeed = 30.0f;
float elevationSpeed = 1.0f;
void Display() {
	//Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Clear color
	glClearColor(0.0, 0.0, 0.6, 1.0);
	//Enable depth testing
	glEnable(GL_DEPTH_TEST);
	//View port
	glViewport(0, 0, WIDTH, HEIGHT);
	//downsample
	//glViewport(0,0,width/2,height/2);

	//Camera update
	controlCamera->computeMatricesFromInputs();

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

	glm::mat4 m = glm::mat4(1.0f);
	m = glm::rotate(m, rot, glm::vec3(0, 1, 0));
	m = glm::translate(m, glm::vec3(0, sin(elevation), 0));
	//m = glm::scale(m, glm::vec3(5.0f));
	//glm::mat4 m = glm::mat4(1.0f);
	glm::mat4 v = controlCamera->getViewMatrix();
	glm::mat3 mn = glm::transpose(glm::inverse(glm::mat3(v*m)));
	glm::mat4 mvp = controlCamera->getProjectionMatrix() * v * m;
	glm::mat4 mv = controlCamera->getViewMatrix() * m;

	glm::mat4 mvp_light = light->getProjMatrix() * light->getViewMatrix() * m;
	//glm::mat3 mn_light = glm::transpose(glm::inverse(glm::mat3(light->getViewMatrix()*m)));
	glm::mat4 m2 = glm::scale(glm::translate(glm::mat4(1.0), glm::vec3(0, -5, 0)), glm::vec3(20));
	glm::mat4 mvp_light2 = light->getProjMatrix() * light->getViewMatrix() * m2;
	//glm::mat3 mn_light2 = glm::transpose(glm::inverse(glm::mat3(light->getViewMatrix()*m2)));

	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1, 1);
	glBindFramebuffer(GL_FRAMEBUFFER, RSMFboManager->getFboId());
	depthShader.Use();
		glViewport(0, 0, SHADOWMAPSIZE, SHADOWMAPSIZE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		light->computeMatrixes();
		glUniformMatrix4fv(depthShader("mvp"), 1, GL_FALSE, glm::value_ptr(mvp_light));
		glUniformMatrix4fv(depthShader("m"), 1, GL_FALSE, glm::value_ptr(m));
		//glUniformMatrix3fv(basicShader("mn"), 1, GL_FALSE, glm::value_ptr(mn_light));
		mesh->render();
		glUniformMatrix4fv(depthShader("mvp"), 1, GL_FALSE, glm::value_ptr(light->getProjMatrix() * light->getViewMatrix() * m2));
		glUniformMatrix4fv(depthShader("m"), 1, GL_FALSE, glm::value_ptr(m2));
		//glUniformMatrix3fv(basicShader("mn"), 1, GL_FALSE, glm::value_ptr(mn_light2));
		mesh2->render();
	depthShader.UnUse();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDisable(GL_POLYGON_OFFSET_FILL);

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
		glUniformMatrix4fv(basicShader("shadowMatrix"), 1, GL_FALSE, glm::value_ptr(biasMatrix*mvp_light2));
		glUniformMatrix4fv(basicShader("mvp"), 1, GL_FALSE, glm::value_ptr(controlCamera->getProjectionMatrix() * v * m2));
		glUniformMatrix4fv(basicShader("mv"), 1, GL_FALSE, glm::value_ptr(controlCamera->getViewMatrix() * glm::mat4(1.0)));
		glUniformMatrix3fv(basicShader("mn"), 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(glm::mat3(v*m2)))));
		mesh2->render();
		glBindTexture(GL_TEXTURE_2D, 0);
	basicShader.UnUse();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Draw quad on screen
	glDisable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ctv2->setTexture(texManager["render_tex"]);
	ctv2->draw();
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
#ifdef CTV
	delete ctv;
	delete ctv2;
#endif
	delete controlCamera;
	delete mesh;
	delete mesh2;
	delete fboManager;
	delete RSMFboManager;
	delete light;
}
void Reshape(int width, int height){
	glViewport(0, 0, width, height);
	aspect = float(height) / float(width);
}


int main() {

	ilutRenderer(ILUT_OPENGL);
	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);

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
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);

	/* Turn on double buffering with a 24bit Z buffer.
	* You may need to change this to 16 or 32 for your system */
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);
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
#ifdef CTV
	w2 = SDL_CreateWindow("Window title goes here #2", 50, 50,
		WIDTH, HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (!w2){ /* Die if creation failed */
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
			if (event.type == SDL_WINDOWEVENT) {
				switch (event.window.event) {
				case SDL_WINDOWEVENT_CLOSE:
					//SDL_Log("Window %d closed", event.window.windowID);
					SDL_DestroyWindow((event.window.windowID > 1) ? w2 : mainwindow);
					break;
				}
			}
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
			light->setPosition(light->getPosition() + (/*glm::vec3(0, 1, 0)*/light->getDirection()* movementSpeed * 2.0f * ftime));
		}
		else if (keys[SDL_SCANCODE_KP_2]) {
			light->setPosition(light->getPosition() - (/*glm::vec3(0, 1, 0)*/light->getDirection()* movementSpeed * 2.0f * ftime));
		}
		else if (keys[SDL_SCANCODE_KP_4]) {
			light->setPosition(light->getPosition() - (/*glm::vec3(1, 0, 0)*/light->getRight()* movementSpeed * 2.f *ftime));
		}
		else if (keys[SDL_SCANCODE_KP_6]) {
			light->setPosition(light->getPosition() + (/*glm::vec3(1, 0, 0)*/light->getRight()* movementSpeed * 2.f* ftime));
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
		SDL_GL_MakeCurrent(w2, maincontext);
		/*ctv->setTexture(texManager["rsm_depth_tex"]);*/
		//rsm_world_space_coords_tex
		//rsm_normal_tex
		ctv->setTexture(texManager["rsm_depth_tex"]);
		ctv->setDepthOnly(true);
		DisplayTexture(ctv);
		SDL_GL_SwapWindow(w2);
#endif

		SDL_GL_MakeCurrent(mainwindow, maincontext);
		Display();
		SDL_GL_SwapWindow(mainwindow);

	}

	Finalize();

	/* Delete our opengl context, destroy our window, and shutdown SDL */
	SDL_GL_DeleteContext(maincontext);
	SDL_DestroyWindow(mainwindow);
#ifdef CTV
	SDL_DestroyWindow(w2);
#endif
	SDL_Quit();

	return 0;
}

