#pragma once
#include "gl.h"

class Lamborghini
{
public:
	Lamborghini() {}
	~Lamborghini() {};

	void Init();
	void Draw();
private:
	class Mesh* M;
	GLuint BodyProgram;
	GLuint ColliderProgram;
	GLuint GlassProgram;
};