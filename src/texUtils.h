/*
* File:   texUtils.h
* Desc:		Utilitky pro praci s texturami
*
*/
#ifndef TEXUTILS_H
#define TEXUTILS_H

#include "common.h"

//DevIL
// Function load a image, turn it into a texture, and return the texture ID as a GLuint for use
// taken from: http://r3dux.org/tag/ilutglloadimage/
GLuint loadImage(const char* theFileName);
//DevIL
std::vector< float > generateRandomSamplingPattern(int numSamples, float r_max );

#endif