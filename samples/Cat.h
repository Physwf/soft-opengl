#pragma once
#include "gl.h"

class Cat
{
public:
	Cat() {}
	~Cat() {};

	void Init();
	void Draw();
private:
	class Mesh* LowPolyCat;
	GLuint LowPolyCatProgram;
};

