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
GLSLShader basicShader;
Mesh * mesh;
float movementSpeed = 4.0f;
float ftime;
GLuint tex;
glm::vec3 lightPosition(0.0, 4.0, 2.0);

//#define CTV

//DevIL
// Function load a image, turn it into a texture, and return the texture ID as a GLuint for use
// taken from: http://r3dux.org/tag/ilutglloadimage/
GLuint loadImage(const char* theFileName)
{
	ILuint imageID;				// Create an image ID as a ULuint

	GLuint textureID;			// Create a texture ID as a GLuint

	ILboolean success;			// Create a flag to keep track of success/failure

	ILenum error;				// Create a flag to keep track of the IL error state

	ilGenImages(1, &imageID); 		// Generate the image ID

	ilBindImage(imageID); 			// Bind the image

	success = ilLoadImage(theFileName); 	// Load the image file

	// If we managed to load the image, then we can start to do things with it...
	if (success)
	{
		// If the image is flipped (i.e. upside-down and mirrored, flip it the right way up!)
		ILinfo ImageInfo;
		iluGetImageInfo(&ImageInfo);
		if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
		{
			iluFlipImage();
		}

		// Convert the image into a suitable format to work with
		// NOTE: If your image contains alpha channel you can replace IL_RGB with IL_RGBA
		success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

		// Quit out if we failed the conversion
		if (!success)
		{
			error = ilGetError();
			std::cout << "Image conversion failed - IL reports error: " << error << " - " << iluErrorString(error) << std::endl;
			exit(-1);
		}

		// Generate a new texture
		glGenTextures(1, &textureID);

		// Bind the texture to a name
		glBindTexture(GL_TEXTURE_2D, textureID);

		// Set texture clamping method
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		// Set texture interpolation method to use linear interpolation (no MIPMAPS)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		// Specify the texture specification
		glTexImage2D(GL_TEXTURE_2D, 				// Type of texture
			0,				// Pyramid level (for mip-mapping) - 0 is the top level
			ilGetInteger(IL_IMAGE_FORMAT),	// Internal pixel format to use. Can be a generic type like GL_RGB or GL_RGBA, or a sized type
			ilGetInteger(IL_IMAGE_WIDTH),	// Image width
			ilGetInteger(IL_IMAGE_HEIGHT),	// Image height
			0,				// Border width in pixels (can either be 1 or 0)
			ilGetInteger(IL_IMAGE_FORMAT),	// Format of image pixel data
			GL_UNSIGNED_BYTE,		// Image data type
			ilGetData());			// The actual image data itself
	}
	else // If we failed to open the image file in the first place...
	{
		error = ilGetError();
		std::cout << "Image load failed - IL reports error: " << error << " - " << iluErrorString(error) << std::endl;
		exit(-1);
	}

	ilDeleteImages(1, &imageID); // Because we have already copied image data into texture data we can release memory used by image.

	std::cout << "Texture creation successful." << std::endl;

	return textureID; // Return the GLuint to the texture so you can use it!
}
//DevIL


void Initialize(SDL_Window * w) {
	tex = loadImage("../textures/texture.png");
#ifdef CTV
	ctv = new CTextureViewer(tex, "../shaders/textureViewer.vs", "../shaders/textureViewer.frag");
	ctv2 = new CTextureViewer(loadImage("../textures/floor.png"), "../shaders/textureViewer.vs", "../shaders/textureViewer.frag");
#endif
	////////////////////////////////////////////////////
	// SHADERS INIT
	////////////////////////////////////////////////////
	basicShader.LoadFromFile(GL_VERTEX_SHADER, std::string("../shaders/basicShader.vs").c_str());
	basicShader.LoadFromFile(GL_FRAGMENT_SHADER, std::string("../shaders/basicShader.frag").c_str());
	basicShader.CreateAndLinkProgram();
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
	basicShader.UnUse();

	////////////////////////////////////////////////////
	// LOAD MODELS
	////////////////////////////////////////////////////
	mesh = new Mesh("../models/cube.obj");
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

	basicShader.Use();
		glm::mat4 m = glm::rotate(glm::mat4(1.0f), rot, glm::vec3(0, 1, 0));
		m = glm::translate(m, glm::vec3(0, sin(elevation), 0));
		//glm::mat4 m = glm::mat4(1.0f);
		glm::mat4 v = controlCamera->getViewMatrix();
		glm::mat3 mn = glm::transpose(glm::inverse(glm::mat3(v*m)));
		glm::mat4 mvp = controlCamera->getProjectionMatrix() * v * m;
		glm::mat4 mv = controlCamera->getViewMatrix() * m;
		glUniformMatrix4fv(basicShader("mvp"), 1, GL_FALSE, glm::value_ptr(mvp));
		glUniformMatrix4fv(basicShader("mv"), 1, GL_FALSE, glm::value_ptr(mv));
		glUniformMatrix4fv(basicShader("v"), 1, GL_FALSE, glm::value_ptr(v));
		glUniformMatrix3fv(basicShader("mn"), 1, GL_FALSE, glm::value_ptr(mn));
		glUniform3f(basicShader("vLightPos"), lightPosition.x, lightPosition.y, lightPosition.z);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex);
		mesh->render();
		glBindTexture(GL_TEXTURE_2D, 0);
	basicShader.UnUse();
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
	//delete ctv;
	//delete ctv2;
	delete controlCamera;
	delete mesh;
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
	w2 = SDL_CreateWindow("Window title goes here #2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
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

	//SDL_GL_SetSwapInterval(1);

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

#ifdef CTV
		SDL_GL_MakeCurrent(w2, maincontext);
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

