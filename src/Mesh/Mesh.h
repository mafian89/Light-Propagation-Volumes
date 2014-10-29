//Taken from website: http://www.nexcius.net/2014/04/13/loading-meshes-using-assimp-in-opengl/
//All credits belong to the author Although I did some changes - #ifdef and include
#ifndef MESH_H
#define	MESH_H
#pragma once

#include "../common.h"

#include <vector>

class Mesh
{
public :
	struct MeshEntry {
		static enum BUFFERS {
			VERTEX_BUFFER, TEXCOORD_BUFFER, NORMAL_BUFFER, INDEX_BUFFER
		};
		GLuint vao;
		GLuint vbo[4];

		unsigned int elementCount;

		MeshEntry(aiMesh *mesh);
		~MeshEntry();

		void load(aiMesh *mesh);
		void render();
	};

	std::vector<MeshEntry*> meshEntries;

public:
	Mesh(const char *filename);
	~Mesh(void);

	void render();
};

#endif
