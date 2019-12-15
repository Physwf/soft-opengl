#include "gl.h"
#include <stdlib.h>
#include "gl_shader.h"
#include "glut.h"

struct VertexShader : public gl_shader
{
	
	struct VS_Input
	{
		vector4 position;
		vector4 color;
	};

	struct VS_Output
	{
		vector4 position;
		vector4 color;
	};

	matrix4 proj;

	VertexShader()
	{
		input_size = sizeof(VS_Input);
		output_size = sizeof(VS_Output);
	}

	GLvoid compile()
	{
		uniforms = gl_uniform_node::create((GLfloat*)&proj,"proj",sizeof(matrix4), GL_FLOAT_MAT4);
	}

	VS_Output process(VS_Input Input)
	{
		VS_Output Out;
		Out.position	= multiply(proj, Input.position) ;
		Out.color		= Input.color;
		return Out;
	}

	VS_Output Output;
	virtual GLvoid* vs_process(GLvoid* Vertex)
	{
		Output = process(*(VS_Input*)Vertex);
		return &Output;
	}
};
struct FragmentShader : public gl_shader
{
	struct PS_Input
	{
		vector4 position;
		vector4 color;
	};

	struct PS_Output
	{
		vector4 color;
	};

	FragmentShader()
	{
		input_size = sizeof(PS_Input);
		output_size = sizeof(PS_Output);
	}

	GLvoid compile()
	{

	}

	PS_Output process(PS_Input Input)
	{
		PS_Output Out;
		Out.color = Input.color;
		//X_LOG("%f,%f,%f\n",Input.color.x, Input.color.y,Input.color.x);
		return Out;
	}

	PS_Output Output;
	virtual GLvoid* fs_process(GLvoid* Vertex)
	{
		Output = process(*(PS_Input*)Vertex);
		return &Output;
	}
};

VertexShader VS;
FragmentShader FS;

void glInit()
{
	glViewport(0, 0, 500, 500);
	glDepthRangef(0.f, 1.0f);
	glClearDepth(0.0f);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

GLfloat points[6*100];
GLfloat lines[3*40];
GLfloat triangles[3 * 6 * 100] = 
{
	0.0f,0.0f,0.0f,			1.0f,	0.0f,	0.0f,
	600.0f,200.0f,0.0f,		0.0f,	0.0f,	1.0f,
	0.0f,200.0f,0.0f,		0.0f,	1.0f,	0.0f,
};

void glSetup()
{
	int i = 0;
	float delta = 5.0f;
	for (int i=0;i<100;++i)
	{
		//Value = (GLfloat)(rand() % 500);
		//Value = (GLfloat)(rand() % 500);
		points[i * 3 + 0] = (i % 10) * 50.0f;
		points[i * 3 + 1] = (i / 10) * 50.0f;
		points[i * 3 + 2] = 200.0f;
	}
	for (int i=0;i<40;++i)
	{
		//lines[i * 3 + 0] = 100;
		//lines[i * 3 + 1] = i*10.0f + 0;
		lines[i * 3 + 2] = 100.0f;
		lines[i * 3 + 0] = (GLfloat)(rand() % 500) + 1.0f;
		lines[i * 3 + 1] = (GLfloat)(rand() % 500) + 1.0f;
	}
// 	lines[0] = 0.0f;
// 	lines[1] = 0.0f;
// 	lines[2] = 0.0f;
// 	lines[3] = 500.0f;
// 	lines[4] = 500.0f;
// 	lines[5] = 100.0f;

// 	lines[0] = 41.0f;
// 	lines[1] = 167.0f;
// 	lines[2] = 100.0f;
// 	lines[3] = 334;
// 	lines[4] = 0.0f;
// 	lines[5] = 100.0f;

	for (int i = 0; i < 300; ++i)
	{
// 		triangles[i * 6 + 0] = (GLfloat)(rand() % 400) + 1.0f;
// 		triangles[i * 6 + 1] = (GLfloat)(rand() % 400) + 1.0f;
// 		triangles[i * 6 + 2] = 100.0f;
// 
// 		triangles[i * 6 + 3] = (GLfloat)(rand() % 500) / 500.0f;
// 		triangles[i * 6 + 4] = (GLfloat)(rand() % 500) / 500.0f;
// 		triangles[i * 6 + 5] = (GLfloat)(rand() % 500) / 500.0f;
	}

	glFrontFace(GL_CW);
	glCullFace(GL_FRONT);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(GLfloat) * 6, triangles);
	glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(GLfloat) * 6, triangles + 3);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	//glVertexAttrib3f(1, 1.0f, 0.0f, 0.0f);
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, &VS);
	glCompileShader(vs);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, &FS);
	GLuint program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glUseProgram(program);

	GLfloat proj[16] = { 0 };
	glutMatrixOrthoLH(proj,  0, 500, 0, 500, 0, 500);
	glUnitformMatrix4fv(0, 1, false, proj);
}

void glRender()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glDrawArrays(GL_TRIANGLE_LIST, 0, 3);

	glFlush();
}
