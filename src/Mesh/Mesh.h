//Taken from website: http://www.nexcius.net/2014/04/13/loading-meshes-using-assimp-in-opengl/
//All credits belong to the author Although I did some changes
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
			VERTEX_BUFFER, TEXCOORD_BUFFER, NORMAL_BUFFER, INDEX_BUFFER/*, MATERIAL_BUFFER*/
		};

		//struct MaterialInfo {
		//	std::string Name;
		//	glm::vec3 Ambient;
		//	glm::vec3 Difuse;
		//	glm::vec3 Specular;
		//	float shininess;
		//	unsigned textureCount;
		//};

		GLuint vao;
		GLuint vbo[4];

		unsigned int elementCount;
		unsigned int materialIndex;

		//Only one material is supported
		//MaterialInfo material;

		MeshEntry(aiMesh *mesh, aiMaterial *mtl);
		~MeshEntry();

		void load(aiMesh *mesh);
		void render();
	};

	struct Texture {
		std::string path;
		GLuint texId;
		GLenum textType;

		void bind(GLenum unit);
		void load(std::string path);

		Texture(std::string path, GLenum textType);
		~Texture();
	};

	std::vector<MeshEntry*> meshEntries;
	std::vector<Texture*> textures;


public:
	Mesh(const char *filename);
	~Mesh(void);

	void render();
};

#endif

