#include "CTextureViewer.h"


CTextureViewer::CTextureViewer()
{
	this->s = new GLSLShader();
	this->fs = "default.frag";
	this->vs = "default.vs";
	setUpShaders();
}


CTextureViewer::CTextureViewer(GLuint tex, std::string vs, std::string fs) {
	this->s = new GLSLShader();
	this->texture = tex;
	this->fs = fs;
	this->vs = vs;
	setUpShaders();
}


CTextureViewer::~CTextureViewer()
{
	delete s;
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &VAO);
}

void CTextureViewer::draw() {
	//Bind VAO
	glBindVertexArray(VAO);
	//Bind VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	this->s->Use();
	
	//Bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->texture);

	//Draw
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	//Unbind texture for sure
	glBindTexture(GL_TEXTURE_2D, 0);

	this->s->UnUse();

	//Unbind VBO - just for sure
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//Unbind VAO
	glBindVertexArray(0);
	
}

//Called only once in constructor
void CTextureViewer::setUpShaders() {
	//We need only vertex and fragment shaders
	s->LoadFromFile(GL_VERTEX_SHADER, this->vs.c_str());
	s->LoadFromFile(GL_FRAGMENT_SHADER, this->fs.c_str());
	s->CreateAndLinkProgram();

	
	s->Use();

	//Create uniforms and attributes (filled later)
	s->AddAttribute("vPosition");
	s->AddAttribute("vUV");
	s->AddUniform("texture");
	

	//Quad verticles - omitted z coord, because it will always be 1
	float pos[] = {
		-1.0, 1.0,
		1.0, 1.0,
		-1.0, -1.0,
		1.0, -1.0
	};

	float uv[] = {
		0.0, 1.0,
		1.0, 1.0,
		0.0, 0.0,
		1.0, 0.0
	};

	//Generate VAO
	glGenVertexArrays(1, &VAO);
	//Bind VAO
	glBindVertexArray(VAO);		

	//Generate VBO
	glGenBuffers(1, &VBO);
	//Bind VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//Alocate buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos)+sizeof(uv), NULL, GL_STATIC_DRAW);
	//Fill VBO
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(pos), pos);
	glBufferSubData(GL_ARRAY_BUFFER, 8 * sizeof(float), sizeof(uv), uv);

	//Fill attributes and uniforms
	glEnableVertexAttribArray((*s)["vPosition"]);
	glVertexAttribPointer((*s)["vPosition"], 2, GL_FLOAT, GL_FALSE, (sizeof(float)* 2), (void*)0);

	glEnableVertexAttribArray((*s)["vUV"]);
	glVertexAttribPointer((*s)["vUV"], 2, GL_FLOAT, GL_FALSE, (sizeof(float)* 2), (GLvoid*)(sizeof(float)* 8));

	glUniform1i((*s)("texture"), 0);

	s->UnUse();
	//unbind buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

//Setters
void CTextureViewer::setTexture(GLuint tex) {
	this->texture = tex;
}
//Getters
GLuint CTextureViewer::getTexture() {
	return this->texture;
}