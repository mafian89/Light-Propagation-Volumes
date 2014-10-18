#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 
#include <SDL2/SDL.h>
//error LNK2019: unresolved external symbol _main referenced in function ___tmainCRTStartup
#undef main
//or use sdlmain.lib
#include "LoadShaders.h"
extern GLuint LoadShaders(ShaderInfo *shaderinfo);

#define PROGRAM_NAME "Red Book - Chapter 3 - Draw Commands"
 
void Initialize();
void Display();
void Finalize(void);
void Reshape(int width, int height);

float aspect;
GLuint render_prog;
GLuint vao;
GLuint vbo;
GLuint ebo;

GLint render_model_matrix_loc;
GLint render_projection_matrix_loc;

void Initialize() {

    ShaderInfo shader_info[] = {
      { GL_VERTEX_SHADER, "../shaders/primitive_restart.vs.glsl" },
      { GL_FRAGMENT_SHADER, "../shaders/primitive_restart.fs.glsl" },
      { GL_NONE, NULL }
    };

  render_prog = LoadShaders(shader_info);
  glUseProgram(render_prog);

  render_model_matrix_loc = glGetUniformLocation(render_prog, "model_matrix");
  render_projection_matrix_loc = glGetUniformLocation(render_prog, "projection_matrix");

  // A single triangle
  static const GLfloat vertex_positions[] = {
      -1.0f, -1.0f,  0.0f, 1.0f,
      1.0f, -1.0f,  0.0f, 1.0f,
      -1.0f,  1.0f,  0.0f, 1.0f,
      -1.0f, -1.0f,  0.0f, 1.0f,
  };

  // Color for each vertex
  static const GLfloat vertex_colors[] = {
      1.0f, 1.0f, 1.0f, 1.0f,
      1.0f, 1.0f, 0.0f, 1.0f,
      1.0f, 0.0f, 1.0f, 1.0f,
      0.0f, 1.0f, 1.0f, 1.0f
  };

  // Indices for the triangle strips
  static const GLushort vertex_indices[] = {
      0, 1, 2
  };

  // Set up the element array buffer
  glGenBuffers(1, &ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertex_indices), vertex_indices, GL_STATIC_DRAW);

  // Set up the vertex attributes
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_positions) + sizeof(vertex_colors), NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertex_positions), vertex_positions);
  glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertex_positions), sizeof(vertex_colors), vertex_colors);

  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid *)sizeof(vertex_positions));
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void Display() {
  glm::mat4 model_matrix;

  // Setup
  glEnable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Activate simple shading program
  glUseProgram(render_prog);

  // Set up the model and projection matrix
  glm::mat4 projection_matrix(glm::frustum(-1.0f, 1.0f, -aspect, aspect, 1.0f, 500.0f));
  glUniformMatrix4fv(render_projection_matrix_loc, 1, GL_FALSE, glm::value_ptr(projection_matrix));

  // Set up for a glDrawElements call
  glBindVertexArray(vao);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

  // Draw Arrays...
  model_matrix = glm::translate(glm::mat4(1.0f),glm::vec3(-3.0f, 0.0f, -5.0f));
  glUniformMatrix4fv(render_model_matrix_loc, 1, GL_FALSE, glm::value_ptr(model_matrix));
  glDrawArrays(GL_TRIANGLES, 0, 3);

  // DrawElements
  model_matrix = glm::translate(glm::mat4(1.0f),glm::vec3(-1.0f, 0.0f, -5.0f));
  glUniformMatrix4fv(render_model_matrix_loc, 1, GL_FALSE, glm::value_ptr(model_matrix));
  glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, NULL);

  // DrawElementsBaseVertex
  model_matrix = glm::translate(glm::mat4(1.0f),glm::vec3(1.0f, 0.0f, -5.0f));
  glUniformMatrix4fv(render_model_matrix_loc, 1, GL_FALSE, glm::value_ptr(model_matrix));
  glDrawElementsBaseVertex(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, NULL, 1);

  // DrawArraysInstanced
  model_matrix = glm::translate(glm::mat4(1.0f),glm::vec3(3.0f, 0.0f, -5.0f));
  glUniformMatrix4fv(render_model_matrix_loc, 1, GL_FALSE, glm::value_ptr(model_matrix));
  glDrawArraysInstanced(GL_TRIANGLES, 0, 3, 1);
}

void Finalize(void) {
  glUseProgram(0);
  glDeleteProgram(render_prog);
  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
}

void Reshape(int width, int height) {
  glViewport(0, 0 , width, height);
  aspect = float(height) / float(width);
}
 
int main(int argc, char *argv[]){
 
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
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
 
  /* Create our window centered at 512x512 resolution */
  mainwindow = SDL_CreateWindow(PROGRAM_NAME, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
				512, 512, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN |SDL_WINDOW_RESIZABLE);
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
  } else {
      std::cout << "GLEW Init: Success!" << std::endl;
	  std::cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;
  }

  /* This makes our buffer swap syncronized with the monitor's vertical refresh */
  
  //SDL_GL_SetSwapInterval(1); 
  
  bool quit=false;

  Initialize();
  Reshape(512,512);

  SDL_Event event;

  while(!quit){

    Display();
    SDL_GL_SwapWindow(mainwindow);

    while( SDL_PollEvent( &event ) ){
	if( event.type == SDL_QUIT ){
            quit = true;
	  }
	if(event.type ==SDL_WINDOWEVENT){
	if(event.window.event = SDL_WINDOWEVENT_SIZE_CHANGED){
		int w,h;
		SDL_GetWindowSize(mainwindow,&w,&h);
		Reshape(w,h);
 	}
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

