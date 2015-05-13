#include "DebugDrawer.h"

DebugDrawer::DebugDrawer() 
: mode(GL_POINTS)
{

}

DebugDrawer::DebugDrawer(GLenum mode, const std::vector<glm::vec3> *p, const std::vector<glm::vec2> *uv, const std::vector<float> *i, glm::vec3 color)
: useIndicies(true), useUv(true), mode(mode), color(color) {

	if (p != NULL) {
		this->p = *p;

		if (uv == NULL)
			useUv = false;
		else
			this->uv = *uv;

		if (i == NULL)
			useIndicies = false;
		else
			this->i = *i;

		setUpShaders();
	}
	else {
		std::cerr << "No vertices given: Nothing to do..." << std::endl;
	}

	//std::cout << useUv << " " << useIndicies << " " << this->p.front().x << "," << this->p.front().y << "," << this->p.front().z << std::endl;
	//std::cout << sizeof(float)* 3 << " vs " << sizeof(glm::vec3) << std::endl;
	//std::cout << this->p.size() * sizeof(glm::vec3) << " + " 
	//	<< this->uv.size() * sizeof(glm::vec2) 
	//	<< " = " << this->p.size() * sizeof(glm::vec3) + this->uv.size() * sizeof(glm::vec2) << std::endl;
}

void DebugDrawer::setVPMatrix(glm::mat4 vp) {
	this->vp = vp;
}

void DebugDrawer::draw(){
	//Bind VAO
	glBindVertexArray(VAO);
	//Bind VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	this->s->Use();
	glUniformMatrix4fv((*s)("vp"), 1, GL_FALSE, glm::value_ptr(vp));
	glUniform3f((*s)("color"), color.x, color.y, color.z);

	//Draw
	glDrawArrays(mode, 0, p.size());

	this->s->UnUse();

	//Unbind VBO - just for sure
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//Unbind VAO
	glBindVertexArray(0);
}

void DebugDrawer::setUpShaders() {
	this->s = new GLSLShader();
	this->vs = "../shaders/debugDraw.vs";
	this->fs = "../shaders/debugDraw.frag";

	//We need only vertex and fragment shaders
	s->LoadFromFile(GL_VERTEX_SHADER, this->vs.c_str());
	s->LoadFromFile(GL_FRAGMENT_SHADER, this->fs.c_str());
	s->CreateAndLinkProgram();
	
	s->Use();

	//Create uniforms and attributes (filled later)
	s->AddAttribute("vPosition");
	s->AddUniform("vp");
	s->AddUniform("color");

	//Generate VAO
	glGenVertexArrays(1, &VAO);
	//Bind VAO
	glBindVertexArray(VAO);

	//Generate VBO
	glGenBuffers(1, &VBO);
	//Bind VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//Alocate buffer
	//glBufferData(GL_ARRAY_BUFFER, this->p.size() * sizeof(glm::vec3) + this->uv.size() * sizeof(glm::vec2), NULL, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, this->p.size() * sizeof(glm::vec3), NULL, GL_DYNAMIC_DRAW);
	//Fill VBO
	glBufferSubData(GL_ARRAY_BUFFER, 0, this->p.size() * sizeof(glm::vec3), &p[0]);
	//if (useUv)
	//	glBufferSubData(GL_ARRAY_BUFFER, this->p.size() * sizeof(glm::vec3), this->uv.size() * sizeof(glm::vec2), &uv[0]);

	//if (useIndicies) {
	//	//EBO
	//}

	//Fill attributes and uniforms
	glEnableVertexAttribArray((*s)["vPosition"]);
	glVertexAttribPointer((*s)["vPosition"], 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

	//glEnableVertexAttribArray((*s)["vUV"]);
	//glVertexAttribPointer((*s)["vUV"], 2, GL_FLOAT, GL_FALSE, (sizeof(float)* 2), (GLvoid*)(sizeof(float)* 8));

	//glUniform1i((*s)("tex"), 0);

	s->UnUse();
	//unbind buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
	
}

void DebugDrawer::updateVBO(const std::vector<glm::vec3> *p) {
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, this->p.size() * sizeof(glm::vec3), &p[0]);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

DebugDrawer::~DebugDrawer() {
	delete s;
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &VAO);
	//glDeleteBuffers(1, &EBO);
}