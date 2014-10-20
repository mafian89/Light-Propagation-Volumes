#include "common.h"
#include "CTextureViewer.h"


#ifdef _MSC_VER
#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib,"devil.lib")
#pragma comment(lib,"ILU.lib")
#pragma comment(lib,"ILUT.lib")
/// etc
#endif

//error LNK2019: unresolved external symbol _main referenced in function ___tmainCRTStartup
#undef main
//or use sdlmain.lib

using namespace std;

float aspect;
CTextureViewer * ctv;

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


void Initialize() {
	ctv = new CTextureViewer(loadImage("../textures/texture.png"), "../shaders/textureViewer.vs", "../shaders/textureViewer.frag");
}
void Display() {

	//glEnable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	ctv->draw();
}
void Finalize(void) {
	delete ctv;
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

	/* Create our window centered at 512x512 resolution */
	mainwindow = SDL_CreateWindow("Window title goes here", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		512, 512, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (!mainwindow){ /* Die if creation failed */
		std::cout << "SDL Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}

	/* Create our opengl context and attach it to our window */
	maincontext = SDL_GL_CreateContext(mainwindow);

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

	Initialize();
	Reshape(512, 512);

	SDL_Event event;

	while (!quit){

		Display();
		SDL_GL_SwapWindow(mainwindow);

		while (SDL_PollEvent(&event)){
			if (event.type == SDL_QUIT){
				quit = true;
			}
		}
	}

	Finalize();

	/* Delete our opengl context, destroy our window, and shutdown SDL */
	SDL_GL_DeleteContext(maincontext);
	SDL_DestroyWindow(mainwindow);
	SDL_Quit();

	return 0;
}

