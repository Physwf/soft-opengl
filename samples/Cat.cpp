#include "Cat.h"
#include "mesh.h"
#include "gl_shader.h"
#include "glut.h"

struct LowPolyCatVertexShader : public gl_shader
{

	struct VS_Input
	{
		vector4 position;
		vector4 normal;
		vector4 TextCoord;
	};

	struct VS_Output
	{
		vector4 position;
		vector4 color;
	};

	matrix4 proj;
	matrix4 model;

	LowPolyCatVertexShader()
	{
		input_size = sizeof(VS_Input);
		output_size = sizeof(VS_Output);
	}

	GLvoid compile()
	{
		uniforms = gl_uniform_node::create((GLfloat*)&proj, "proj", sizeof(matrix4), GL_FLOAT_MAT4);
		uniforms->next = gl_uniform_node::create((GLfloat*)&model, "model", sizeof(matrix4), GL_FLOAT_MAT4);
	}

	VS_Output process(VS_Input Input)
	{
		VS_Output Out;
		Out.position = multiply(model, Input.position);
		Out.position = multiply(proj, Out.position);
		vector4 normal = multiply(model, Input.normal);
		float fDot = dot(normal.to_vector3(), vector3(0.0f, 0, -1.0f));
		fDot = clamp(fDot);
		Out.color = vector4(fDot * 0.6f, fDot * 0.6f, fDot * 0.6f, 1.0f);
		return Out;
	}

	VS_Output Output;
	virtual GLvoid* vs_process(GLvoid* Vertex)
	{
		Output = process(*(VS_Input*)Vertex);
		return &Output;
	}
};
struct LowPolyCatFragmentShader : public gl_shader
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

	LowPolyCatFragmentShader()
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
	virtual GLvoid* fs_process(GLvoid* Input, struct gl_rs_state* rs, GLsizei screenx, GLsizei screeny) override
	{
		Output = process(*(PS_Input*)Input);
		return &Output;
	}
};

LowPolyCatVertexShader LowPolyCatVS;
LowPolyCatFragmentShader LowPolyCatFS;

void Cat::Init()
{
	LowPolyCat = new Mesh();
	LowPolyCat->LoadFromObj("./lowpolycat/cat.obj");


	glViewport(0, 0, 500, 500);
	glDepthRangef(0.f, 1.0f);
	glClearDepth(1.0f);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

	//glVertexAttrib3f(1, 1.0f, 0.0f, 0.0f);
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, &LowPolyCatVS);
	glCompileShader(vs);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, &LowPolyCatFS);
	LowPolyCatProgram = glCreateProgram();
	glAttachShader(LowPolyCatProgram, vs);
	glAttachShader(LowPolyCatProgram, fs);
	glLinkProgram(LowPolyCatProgram);
}

void Cat::Draw()
{
	if (LowPolyCat)
	{
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		for (auto&& Pair : LowPolyCat->GetSubMeshes())
		{
			const Mesh::SubMesh& Sub = Pair.second;
			Sub.Vertices;
			glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Mesh::Vertex), Sub.Vertices.data());
			glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Mesh::Vertex), ((unsigned char*)Sub.Vertices.data()) + sizeof(Vector));
			glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(Mesh::Vertex), ((unsigned char*)Sub.Vertices.data()) + 2*sizeof(Vector));
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
			glUseProgram(LowPolyCatProgram);
			GLfloat proj[16] = { 0 };
			glutMatrixOrthoLH(proj, -549, 450, -199, 800, -300, 1500);
			glUnitformMatrix4fv(0, 1, false, proj);
			GLfloat model[16] = { 0 };
			static float rad = 0.0f;
			rad += 0.01f;
			glutMatrixRotationY(model, rad);
			glUnitformMatrix4fv(1, 1, false, model);

			glDrawElements(GL_TRIANGLE_LIST, Sub.Indices.size(),GL_SHORT,Sub.Indices.data());
		}
		glFlush();
	}
}

