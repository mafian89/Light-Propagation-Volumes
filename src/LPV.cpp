#include "common.h"
#include "CTextureViewer.h"


#ifdef _MSC_VER
#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "glew32.lib")
/// etc
#endif

//error LNK2019: unresolved external symbol _main referenced in function ___tmainCRTStartup
#undef main
//or use sdlmain.lib

using namespace std;

float aspect;
CTextureViewer * ctv;

void Initialize() {
	ctv = new CTextureViewer((GLuint)0, "../shaders/textureViewer.vs", "../shaders/textureViewer.fs");
}
void Display() {

	//glEnable(GL_CULL_FACE);
	//glDisable(GL_DEPTH_TEST);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	
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

	
	//cout << ctv->sayHello() << " " << ctv->getTexture() << endl;
	
	

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

