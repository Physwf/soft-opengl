#include "Lamborghini.h"
#include "mesh.h"
#include "gl_shader.h"
#include "glut.h"
#include "light.h"
#include "material.h"
#include <cmath>

struct LamborghiniVertexShader : public gl_shader
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
		vector4 normal;
		vector4 TextCoord;
	};

	matrix4 proj;
	matrix4 model;

	LamborghiniVertexShader()
	{
		//size = sizeof(LamborghiniVertexShader);
		input_size = sizeof(VS_Input);
		output_size = sizeof(VS_Output);
	}

	GLvoid compile()
	{
		create_uniform((GLfloat*)&proj, "proj", sizeof(matrix4), GL_FLOAT_MAT4);
		create_uniform((GLfloat*)&model, "model", sizeof(matrix4), GL_FLOAT_MAT4);
	}

	VS_Output process(VS_Input Input)
	{
		VS_Output Out;
		Out.position = multiply(model, Input.position);
		Out.position = multiply(proj, Out.position);
		Out.normal = multiply(model, Input.normal);
		Out.TextCoord = Input.TextCoord;
		return Out;
	}

	VS_Output Output;
	virtual GLvoid* vs_process(GLvoid* Vertex)
	{
		Output = process(*(VS_Input*)Vertex);
		return &Output;
	}
};

struct LamborghiniFragmentShaderBase : public gl_shader
{
	struct AmbientLight
	{
		vector4 Color;
	};
	struct DirectionalLight
	{
		vector3 Direction;
		vector4 Color;
	};
	struct PointLight
	{
		vector3 Position;
		vector3 Color;
	};
	struct Material
	{
		vector3 Ka;
		vector3 Kd;
		vector3 Ks;
		vector3 Tf;
		float Ni;
		float Ns;
	};

	struct PS_Input
	{
		vector4 position;
		vector4 normal;
		vector4 TextCoord;
	};

	struct PS_Output
	{
		vector4 color;
	};

	LamborghiniFragmentShaderBase()
	{
		//size = sizeof(LamborghiniFragmentShader);
		input_size = sizeof(PS_Input);
		output_size = sizeof(PS_Output);
	}

	virtual GLvoid compile()
	{
		create_uniform((GLfloat*)&AL.Color, "AL.Color", sizeof(AL.Color), GL_FLOAT_VEC4);
		create_uniform((GLfloat*)&DL.Color, "DL.Color", sizeof(DL.Color), GL_FLOAT_VEC4);
		create_uniform((GLfloat*)&DL.Direction, "DL.Direction", sizeof(DL.Direction), GL_FLOAT_VEC3);
		create_uniform((GLfloat*)&PLs[0].Position, "PLs[0].Position", sizeof(PLs[0].Position), GL_FLOAT_VEC3);
		create_uniform((GLfloat*)&PLs[0].Color, "PLs[0].Color", sizeof(PLs[0].Color), GL_FLOAT_VEC3);
		create_uniform((GLfloat*)&PLs[1].Position, "PLs[1].Position", sizeof(PLs[1].Position), GL_FLOAT_VEC3);
		create_uniform((GLfloat*)&PLs[1].Color, "PLs[1].Color", sizeof(PLs[1].Color), GL_FLOAT_VEC3);
		create_uniform((GLfloat*)&PLs[2].Position, "PLs[2].Position", sizeof(PLs[2].Position), GL_FLOAT_VEC3);
		create_uniform((GLfloat*)&PLs[2].Color, "PLs[2].Color", sizeof(PLs[2].Color), GL_FLOAT_VEC3);
		create_uniform((GLfloat*)&PLs[3].Position, "PLs[3].Position", sizeof(PLs[3].Position), GL_FLOAT_VEC3);
		create_uniform((GLfloat*)&PLs[3].Color, "PLs[3].Color", sizeof(PLs[3].Color), GL_FLOAT_VEC3);
	}

	virtual const vector4* get_attribute_texcoord0(GLsizei x, GLsizei y) override
	{
		if (x <= 0 || y < 0) return nullptr;
		if (x >= screen_width || y >= screen_height) return nullptr;
		const vector4* attribute = (const vector4*)get_varing_attribute(x, y);
		if(attribute != nullptr) return attribute + 2;
		return nullptr;
	}
	
	PS_Output Output;

	

	Material material;

	AmbientLight AL;
	DirectionalLight DL;
	PointLight PLs[4];

	
};

struct LamborghiniFragmentShaderBody : public LamborghiniFragmentShaderBase
{
	virtual GLvoid compile()
	{
		LamborghiniFragmentShaderBase::compile();

		create_uniform((GLuint*)&diffuseSampler2D, "diffuseSampler2D", sizeof(sampler2D), GL_UNSIGNED_INT);
		create_uniform((GLuint*)&specularSampler2D, "specularSampler2D", sizeof(sampler2D), GL_UNSIGNED_INT);

		create_uniform((GLfloat*)&material.Ka, "material.Ka", sizeof(material.Ka), GL_FLOAT_VEC3);
		create_uniform((GLfloat*)&material.Kd, "material.Kd", sizeof(material.Kd), GL_FLOAT_VEC3);
		create_uniform((GLfloat*)&material.Tf, "material.Tf", sizeof(material.Tf), GL_FLOAT_VEC3);
		create_uniform((GLfloat*)&material.Ks, "material.Ks", sizeof(material.Ks), GL_FLOAT_VEC3);
		create_uniform((GLfloat*)&material.Ni, "material.Ni", sizeof(material.Ni), GL_FLOAT);
		create_uniform((GLfloat*)&material.Ns, "material.Ns", sizeof(material.Ns), GL_FLOAT);
	}

	PS_Output process(PS_Input Input)
	{
		PS_Output Out;

		vector3 normal = normalize(Input.normal.to_vector3());
		vector3 light_dir = normalize(DL.Direction);

		//ambient
		vector3 ambient = multiply(AL.Color.to_vector3(), material.Ka);
		//diffuse
		float diffuseLight = maxf(dot(normal, light_dir), 0.0f);
		vector4 diffuseTex = texture(diffuseSampler2D, (vector2*)&Input.TextCoord);
		vector3 diffuse =  multiply(add(diffuseTex.to_vector3(),material.Ka), multiply(diffuseLight, DL.Color.to_vector3()));
		//specular
		vector3 half = add(light_dir , vector3(0.0f, 0.0f, 1.0f));
		half = normalize(half);
		float specularLight = std::powf(maxf(dot(normal, half), 0.0f), material.Ns);
		if (diffuseLight <= 0) specularLight = 0.0f;
		vector4 specularTex = texture(specularSampler2D, (vector2*)&Input.TextCoord);
		vector3 specular = multiply(specularLight, multiply(DL.Color.to_vector3(), add(material.Ks, diffuseTex.to_vector3())));

		for (int i = 0; i < 4; ++i)
		{
			vector3 LPos = PLs[i].Position;
			vector4 LColor = vector4(1.0f, 1.0f, 1.0f, 1.0f);
			vector3 LDir = sub(LPos, Input.position.to_vector3());
			float len = length(LDir);
			LDir = normalize(LDir);
			diffuseLight = maxf(dot(normal, LDir), 0.0f);
			diffuseLight *= (50.0f/len);
			diffuse = add(diffuse, multiply(add(diffuseTex.to_vector3(), material.Ka), multiply(diffuseLight, LColor.to_vector3())));
		}

		vector3 color = add(add(ambient, diffuse),specular);
		Out.color = vector4(color.x, color.y, color.z, 1.0f);
		return Out;
	}

	virtual GLvoid* fs_process(GLvoid* Vertex,struct gl_rs_state* rs, GLsizei screenx, GLsizei screeny)
	{
		gl_shader::fs_process(Vertex, rs, screenx, screeny);
		Output = process(*(PS_Input*)Vertex);
		return &Output;
	}

	sampler2D diffuseSampler2D;
	sampler2D specularSampler2D;
};

struct LamborghiniFragmentShaderCollider : public LamborghiniFragmentShaderBase
{
	virtual GLvoid compile()
	{
		LamborghiniFragmentShaderBase::compile();

		create_uniform((GLfloat*)&material.Ka, "material.Ka", sizeof(material.Ka), GL_FLOAT_VEC3);
		create_uniform((GLfloat*)&material.Kd, "material.Kd", sizeof(material.Kd), GL_FLOAT_VEC3);
		create_uniform((GLfloat*)&material.Tf, "material.Tf", sizeof(material.Tf), GL_FLOAT_VEC3);
		create_uniform((GLfloat*)&material.Ni, "material.Ni", sizeof(material.Ni), GL_FLOAT);
	}

	PS_Output process(PS_Input Input)
	{
		PS_Output Out;
		//ambient
		vector3 ambientColor = multiply(AL.Color.to_vector3(), material.Ka);
		//diffuse
		vector3 normal = normalize(Input.normal.to_vector3());
		vector3 light_dir = normalize(DL.Direction);
		float lamb = clamp(dot(normal, light_dir));
		vector3 lambColor = multiply(material.Kd, multiply(lamb, DL.Color.to_vector3()));
		//specular
		vector3 half = add(light_dir, vector3(0.0f, 0.0f, -1.0f));
		half = normalize(half);
		float ndoth = clamp(dot(normal, half));
		float intensity = std::powf(ndoth, material.Ns);
		vector3 specularColor = multiply(intensity, material.Ks);

		vector3 color = add(ambientColor, lambColor);
		color = add(specularColor, color);

		Out.color = vector4(color.x, color.y, color.z, 1.0f);
		return Out;
	}

	virtual GLvoid* fs_process(GLvoid* Vertex, struct gl_rs_state* rs, GLsizei screenx, GLsizei screeny)
	{
		gl_shader::fs_process(Vertex, rs, screenx, screeny);
		Output = process(*(PS_Input*)Vertex);
		return &Output;
	}
};

struct LamborghiniFragmentShaderGlass : public LamborghiniFragmentShaderBase
{
	virtual GLvoid compile()
	{
		LamborghiniFragmentShaderBase::compile();

		create_uniform((GLfloat*)&material.Ka, "material.Ka", sizeof(material.Ka), GL_FLOAT_VEC3);
		create_uniform((GLfloat*)&material.Kd, "material.Kd", sizeof(material.Kd), GL_FLOAT_VEC3);
		create_uniform((GLfloat*)&material.Tf, "material.Tf", sizeof(material.Tf), GL_FLOAT_VEC3);
		create_uniform((GLfloat*)&material.Ks, "material.Ks", sizeof(material.Ks), GL_FLOAT_VEC3);
		create_uniform((GLfloat*)&material.Ni, "material.Ni", sizeof(material.Ni), GL_FLOAT);
		create_uniform((GLfloat*)&material.Ns, "material.Ns", sizeof(material.Ns), GL_FLOAT);
	}


	PS_Output process(PS_Input Input)
	{
		PS_Output Out;
		//ambient
		vector3 ambientColor = multiply(AL.Color.to_vector3(), material.Ka);
		//diffuse
		vector3 normal = normalize(Input.normal.to_vector3());
		vector3 light_dir = normalize(DL.Direction);
		float lamb = clamp(dot(normal, light_dir));
		vector3 lambColor = multiply(material.Kd, multiply(lamb, DL.Color.to_vector3()));
		//specular
		vector3 half = add(light_dir, vector3(0.0f, 0.0f, -1.0f));
		half = normalize(half);
		float ndoth = clamp(dot(normal, half));
		float intensity = std::powf(ndoth, material.Ns);
		vector3 specularColor = multiply(intensity, material.Ks);

		vector3 color = add(ambientColor, lambColor);
		color = add(specularColor, color);

		Out.color = vector4(color.x, color.y, color.z, 1.0f);
		return Out;
	}

	virtual GLvoid* fs_process(GLvoid* Vertex, struct gl_rs_state* rs, GLsizei screenx, GLsizei screeny)
	{
		gl_shader::fs_process(Vertex, rs, screenx, screeny);
		Output = process(*(PS_Input*)Vertex);
		return &Output;
	}
};

LamborghiniVertexShader LamborghiniVS;
LamborghiniFragmentShaderBody LamborghiniBodyFS;
LamborghiniFragmentShaderCollider LamborghiniColliderFS;
LamborghiniFragmentShaderGlass LamborghiniGlassFS;

void Lamborghini::Init()
{
	M = new Mesh();
	M->LoadFromObj("./Lamborginhi Aventador OBJ/Lamborghini_Aventador.obj");


	glViewport(0, 0, 500, 500);
	glDepthRangef(0.f, 1.0f);
	glClearDepth(1.0f);
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, &LamborghiniVS);
	glCompileShader(vs);

	GLuint body_fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(body_fs, &LamborghiniBodyFS);
	glCompileShader(body_fs);

	BodyProgram = glCreateProgram();
	glAttachShader(BodyProgram, vs);
	glAttachShader(BodyProgram, body_fs);
	glLinkProgram(BodyProgram);

	GLuint collider_fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(collider_fs, &LamborghiniColliderFS);
	glCompileShader(collider_fs);

	ColliderProgram = glCreateProgram();
	glAttachShader(ColliderProgram, vs);
	glAttachShader(ColliderProgram, collider_fs);
	glLinkProgram(ColliderProgram);

	GLuint glass_fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(glass_fs, &LamborghiniGlassFS);
	glCompileShader(glass_fs);

	GlassProgram = glCreateProgram();
	glAttachShader(GlassProgram, vs);
	glAttachShader(GlassProgram, glass_fs);
	glLinkProgram(GlassProgram);
}

void Lamborghini::Draw()
{
	if (M)
	{
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		

		int i = 0;
		for (auto& Pair : M->GetSubMeshes())
		{
			const Mesh::SubMesh& Sub = Pair.second;
			++i;
			if(i==2) continue;
			glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Mesh::Vertex), Sub.Vertices.data());
			glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Mesh::Vertex), ((unsigned char*)Sub.Vertices.data()) + sizeof(Vector));
			glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(Mesh::Vertex), ((unsigned char*)Sub.Vertices.data()) + 2 * sizeof(Vector));
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
			const Material& Mtl = M->GetMaterial(Sub.MaterialName);
			GLuint Program;
			if (Sub.MaterialName == "Lamborghini_Aventador:BodySG")
			{
				Program = BodyProgram;
				glUseProgram(Program);

				GLint loc = glGetUniformLocation(Program, "material.Ka");
				glUniform4fv(loc, 1, &Mtl.Ka.r);
				loc = glGetUniformLocation(Program, "material.Kd");
				glUniform4fv(loc, 1, &Mtl.Kd.r);
				loc = glGetUniformLocation(Program, "material.Ks");
				glUniform4fv(loc, 1, &Mtl.Ks.r);
				loc = glGetUniformLocation(Program, "material.Tf");
				glUniform4fv(loc, 1, &Mtl.Tf.r);
				loc = glGetUniformLocation(Program, "material.Ni");
				glUniform1f(loc,Mtl.Ni);
				loc = glGetUniformLocation(Program, "material.Ns");
				glUniform1f(loc,Mtl.Ns);

				const Texture2D* diffuseTexture = M->GetTexture2D(Mtl.map_Kd);
				const Texture2D* specularTexture = M->GetTexture2D(Mtl.map_Ks);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, diffuseTexture->GetHandle());
				loc = glGetUniformLocation(Program, "diffuseSampler2D");
				glUniform1i(loc, 0);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, specularTexture->GetHandle());
				loc = glGetUniformLocation(Program, "specularSampler2D");
				glUniform1i(loc, 1);
			}
			else if(Sub.MaterialName == "Lamborghini_Aventador:ColliderSG")
			{
				Program = ColliderProgram;
				glUseProgram(Program);
				GLint loc = glGetUniformLocation(Program, "material.Ka");
				glUniform4fv(loc, 1, &Mtl.Ka.r);
				loc = glGetUniformLocation(Program, "material.Kd");
				glUniform4fv(loc, 1, &Mtl.Kd.r);
				loc = glGetUniformLocation(Program, "material.Tf");
				glUniform4fv(loc, 1, &Mtl.Tf.r);
				loc = glGetUniformLocation(Program, "material.Ni");
				glUniform1f(loc, Mtl.Ni);
			}
			else if (Sub.MaterialName == "Lamborghini_Aventador:GlassSG")
			{
				Program = GlassProgram;
				glUseProgram(Program);
				GLint loc = glGetUniformLocation(Program, "material.Ka");
				glUniform4fv(loc, 1, &Mtl.Ka.r);
				loc = glGetUniformLocation(Program, "material.Kd");
				glUniform4fv(loc, 1, &Mtl.Kd.r);
				loc = glGetUniformLocation(Program, "material.Ks");
				glUniform4fv(loc, 1, &Mtl.Ks.r);
				loc = glGetUniformLocation(Program, "material.Tf");
				glUniform4fv(loc, 1, &Mtl.Tf.r);
				loc = glGetUniformLocation(Program, "material.Ni");
				glUniform1f(loc, Mtl.Ni);
				loc = glGetUniformLocation(Program, "material.Ns");
				glUniform1f(loc, Mtl.Ns);
			}

			{
				GLfloat proj[16] = { 0 };
				glutMatrixOrthoLH(proj, -250, 249, -200, 299, -300, 1500);
				GLint loc = glGetUniformLocation(Program, "proj");
				glUnitformMatrix4fv(loc, 1, false, proj);
				GLfloat model[16] = { 0 };
				static float rad = 0.0f;
				rad += 0.01f;
				glutMatrixRotationY(model, rad);
				loc = glGetUniformLocation(Program, "model");
				glUnitformMatrix4fv(loc, 1, false, model);
			}


			{
				AmbientLight AL;
				AL.Color = LinearColor(0.4f, 0.4f, 0.4f, 1.0f);
				DirctionalLight DL;
				DL.Color = LinearColor(1.0f, 1.0f, 1.0f, 1.0f);
				DL.Direction = { 0.0f, -1.0, 1.0f };
				PointLight PLs[4];
				PLs[0].Position = { -100.0f, 50.0, 100.0f };
				PLs[1].Position = { -100.0f, 50.0, 500.0f };
				PLs[2].Position = { 100.0f, 50.0, 100.0f };
				PLs[3].Position = { 100.0f, 50.0, 500.0f };
				PLs[0].Color = LinearColor(1.0f, 1.0f, 1.0f, 1.0f);
				PLs[1].Color = LinearColor(1.0f, 1.0f, 1.0f, 1.0f);
				PLs[2].Color = LinearColor(1.0f, 1.0f, 1.0f, 1.0f);
				PLs[3].Color = LinearColor(1.0f, 1.0f, 1.0f, 1.0f);

				GLint loc = glGetUniformLocation(Program, "AL.Color");
				glUniform4fv(loc, 1, &AL.Color.r);
				loc = glGetUniformLocation(Program, "DL.Color");
				glUniform4fv(loc, 1, &DL.Color.r);
				loc = glGetUniformLocation(Program, "DL.Direction");
				glUniform3fv(loc, 1, &DL.Direction.x);
				loc = glGetUniformLocation(Program, "PLs[0].Position");
				glUniform3fv(loc, 1, &(PLs[0].Position.x));
				loc = glGetUniformLocation(Program, "PLs[0].Color");
				glUniform4fv(loc, 1, &(PLs[0].Color.r));
				loc = glGetUniformLocation(Program, "PLs[1].Position");
				glUniform3fv(loc, 1, &(PLs[1].Position.x));
				loc = glGetUniformLocation(Program, "PLs[1].Color");
				glUniform4fv(loc, 1, &(PLs[1].Color.r));
				loc = glGetUniformLocation(Program, "PLs[2].Position");
				glUniform3fv(loc, 1, &(PLs[2].Position.x));
				loc = glGetUniformLocation(Program, "PLs[2].Color");
				glUniform4fv(loc, 1, &(PLs[2].Color.r));
				loc = glGetUniformLocation(Program, "PLs[3].Position");
				glUniform3fv(loc, 1, &(PLs[3].Position.x));
// 				loc = glGetUniformLocation(Program, "PLs[3].Color");
// 				glUniform4fv(loc, 1, &(PLs[3].Color.r));
			}
			

			glDrawElements(GL_TRIANGLE_LIST, Sub.Indices.size(), GL_SHORT, Sub.Indices.data());
		}
		glFlush();
	}
}
