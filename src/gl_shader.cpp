/* Copyright(C) 2019-2020 Physwf

This program is free software : you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.If not, see < http://www.gnu.org/licenses/>.
*/

#include "gl_frontend.h"
#include "gl_shader.h"
#include "gl_memory.h"
#include "gl_pipeline.h"

#include <string>

gl_program_object* gl_find_program_object_check(GLuint name)
{
	gl_named_object_node* node = gl_find_named_object(name);
	if (node == nullptr)
	{
		glSetError(GL_INVALID_VALUE, "Invalid program name!");
		return nullptr;
	}
	gl_named_object* object = node->object;
	if (object == nullptr)
	{
		glSetError(GL_INVALID_VALUE, "Invalid program name!");
		return nullptr;
	}
	if (object->type != __PROGRAM_OBJECT__)
	{
		glSetError(GL_INVALID_VALUE, "Invalid program name!");
		return nullptr;
	}
	return (gl_program_object*)node->object;
}

void gl_destroy_named_object(GLuint name)
{
	gl_named_object_node* node = glPipeline.named_object_list;
	while (node && node->next)
	{
		gl_named_object_node* prenode = node;
		node = node->next;
		if (node->object->name == name)
		{
			prenode->next = node->next;
			node->next = nullptr;
			gl_free(node);
			break;
		}
	}
	glSetError(GL_INVALID_VALUE, "Invalid shader name!");
}


gl_program_object* gl_find_program_object(GLuint name)
{
	gl_named_object_node* node = gl_find_named_object(name);
	if (node == nullptr) return nullptr;
	gl_named_object* object = node->object;
	if (object == nullptr) return nullptr;
	if (object->type != __PROGRAM_OBJECT__)
	{
		return nullptr;
	}
	return (gl_program_object*)object;
}

 GLuint glCreateShader(GLenum type)
{
	switch (type)
	{
	case GL_VERTEX_SHADER:
		return gl_create_named_object(__VERTEX_SHADER_OBJECT__);
	case GL_FRAGMENT_SHADER:
		return gl_create_named_object(__FRAGMENT_SHADER_OBJECT__);
	default:
		glSetError(GL_INVALID_ENUM, "invalid shader type");
	}
	return 0;
}

 void glCompileShader(GLuint shader)
{
	gl_named_object_node* node = gl_find_named_object(shader);
	if (node == nullptr)
	{
		glSetError(GL_INVALID_VALUE, "Invalid shader name!");
		return;
	}
	gl_named_object* object = node->object;
	if (object == nullptr)
	{
		glSetError(GL_INVALID_VALUE, "Invalid shader name!");
		return;
	}
	if (object->type != __VERTEX_SHADER_OBJECT__ && object->type != __FRAGMENT_SHADER_OBJECT__)
	{
		glSetError(GL_INVALID_VALUE, "Invalid shader name!");
		return;
	}
	gl_shader_object* shader_object = (gl_shader_object*)object;
	shader_object->shader->compile();
}

 void glShaderSource(GLuint shader, GLvoid* source)
{
	gl_named_object_node* node = gl_find_named_object(shader);
	if (node == nullptr)
	{
		glSetError(GL_INVALID_VALUE, "Invalid shader name!");
		return;
	}
	gl_named_object* object = node->object;
	if (object == nullptr)
	{
		glSetError(GL_INVALID_VALUE, "Invalid shader name!");
		return;
	}
	if (object->type != __VERTEX_SHADER_OBJECT__ && object->type != __FRAGMENT_SHADER_OBJECT__)
	{
		glSetError(GL_INVALID_VALUE, "Invalid shader name!");
		return;
	}
	gl_shader_object* shader_object = (gl_shader_object*)object;
	shader_object->shader = (gl_shader*)source;
}

 void glDeleteShader(GLuint shader)
{
	gl_destroy_named_object(shader);
}

 GLuint glCreateProgram()
{
	return gl_create_named_object(__PROGRAM_OBJECT__);
}

 void glAttachShader(GLuint program, GLuint shader)
{
	gl_program_object* program_object = gl_find_program_object_check(program);
	if (program_object == nullptr) return;

	gl_named_object_node* shader_node = gl_find_named_object(shader);
	if (shader_node == nullptr)
	{
		glSetError(GL_INVALID_VALUE, "Invalid shader name!");
		return;
	}
	gl_shader_object* shader_object = (gl_shader_object*)shader_node->object;
	if (shader_object == nullptr)
	{
		glSetError(GL_INVALID_VALUE, "Invalid shader name!");
		return;
	}

	if (shader_object->type == __VERTEX_SHADER_OBJECT__)
	{
		if (program_object->vertex_shader_object != nullptr)
		{
			glSetError(GL_INVALID_OPERATION, "Program have already have vertex shader!");
			return;
		}
		else
		{
			program_object->vertex_shader_object = shader_object;
		}
	}
	else if (shader_object->type == __FRAGMENT_SHADER_OBJECT__)
	{
		if (program_object->fragment_shader_object != nullptr)
		{
			glSetError(GL_INVALID_OPERATION, "Program have already have fragment shader!");
			return;
		}
		else
		{
			program_object->fragment_shader_object = shader_object;
		}
	}
}

void glDetachShader(GLuint program, GLuint shader)
{
	gl_program_object* program_object = gl_find_program_object_check(program);
	if (program_object == nullptr) return;
	
	gl_named_object_node* shader_node = gl_find_named_object(shader);
	if (shader_node == nullptr)
	{
		glSetError(GL_INVALID_VALUE, "Invalid shader name!");
		return;
	}
	gl_shader_object* shader_object = (gl_shader_object*)shader_node->object;
	if (shader_object == nullptr)
	{
		glSetError(GL_INVALID_VALUE, "Invalid shader name!");
		return;
	}

	if (shader_object->type == __VERTEX_SHADER_OBJECT__)
	{
		if (program_object->vertex_shader_object != shader_object)
		{
			glSetError(GL_INVALID_OPERATION, "Program have already have vertex shader!");
			return;
		}
		else
		{
			program_object->vertex_shader_object = nullptr;
		}
	}
	else if (shader_object->type == __FRAGMENT_SHADER_OBJECT__)
	{
		if (program_object->fragment_shader_object != shader_object)
		{
			glSetError(GL_INVALID_OPERATION, "Program have already have fragment shader!");
			return;
		}
		else
		{
			program_object->fragment_shader_object = nullptr;
		}
	}
}

 void glLinkProgram(GLuint program)
{
	gl_program_object* program_object = gl_find_program_object_check(program);
	if (program_object == nullptr) return;

	gl_shader_object* vs_object = program_object->vertex_shader_object;
	gl_shader_object* fs_object = program_object->fragment_shader_object;
	if (vs_object == nullptr || fs_object == nullptr)
	{
		glSetError(GL_INVALID_OPERATION, "shader is not attached!");
		return;
	}
	gl_shader* vs = vs_object->shader;
	gl_shader* fs = fs_object->shader;
	if (vs == nullptr || fs == nullptr)
	{
		glSetError(GL_INVALID_OPERATION, "shader has no source!");
		return;
	}
	GLsizei index = 0;

	gl_uniform_node* uniform_node = vs->uniforms;
	while (uniform_node)
	{
		program_object->uniforms[index] = uniform_node->uniform;
		uniform_node = uniform_node->next;
		++index;
		if (index >= MAX_UNIFORMS)
		{
			glSetError(GL_INVALID_VALUE, "Too much uniform!");
			return;
		}
	}
	uniform_node = fs->uniforms;
	while (uniform_node)
	{
		program_object->uniforms[index] = uniform_node->uniform;
		uniform_node = uniform_node->next;
		++index;
		if (index >= MAX_UNIFORMS)
		{
			glSetError(GL_INVALID_VALUE, "Too much uniform!");
			return;
		}
	}
	program_object->active_uniforms = index;
}

 void glUseProgram(GLuint program)
{
	if (gl_find_program_object_check(program) == nullptr) return;

	glContext.program = program;
}

 void glDeleteProgram(GLuint program)
{

}

 GLint glGetUniformLocation(GLuint program, const GLchar* name)
{
	gl_program_object* program_object = gl_find_program_object_check(program);
	gl_named_object_node* node = gl_find_named_object(program);
	if (program_object == nullptr) return -1;
	
	for (GLuint i = 0; i < program_object->active_uniforms; ++i)
	{
		gl_uniform* uniform = program_object->uniforms[i];
		if (uniform == nullptr)
		{
			return -1;//this should not happen
		}
		if (strcmp(uniform->name, name) == 0) return i;
	}
	return -1;
}

 void glGetActiveUniform(GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, GLchar* name)
{
	gl_program_object* program_object = gl_find_program_object_check(program);
	if (program_object == nullptr) return;
	if (index >= program_object->active_uniforms)
	{
		glSetError(GL_INVALID_VALUE, "Invalid index!");
		return;
	}
	gl_uniform* uniform = program_object->uniforms[index];
	if (length != nullptr) *length = strlen(uniform->name);
	if (size != nullptr) *size = uniform->size;
	if (type != nullptr) *type = uniform->size;
	if (name != nullptr) strcpy_s(name, bufsize, uniform->name); 
}

GLfloat* gl_get_uniform_f_check(GLuint program, GLint location)
{
	if (program == 0)
	{
		glSetError(GL_INVALID_OPERATION, "No program is used!");
		return nullptr;
	}
	gl_program_object* program_object = gl_find_program_object_check(program);
	if (program_object == nullptr) return nullptr;
	if (location >= (GLint)program_object->active_uniforms)
	{
		glSetError(GL_INVALID_VALUE, "Invalid index!");
		return nullptr;
	}
	return (GLfloat*)program_object->uniforms[location]->fvalue;
}

GLint* gl_get_uniform_i_check(GLuint program, GLint location)
{
	if (program == 0)
	{
		glSetError(GL_INVALID_OPERATION, "No program is used!");
		return nullptr;
	}
	gl_program_object* program_object = gl_find_program_object_check(program);
	if (program_object == nullptr) return nullptr;
	if (location >= (GLint)program_object->active_uniforms)
	{
		glSetError(GL_INVALID_VALUE, "Invalid index!");
		return nullptr;
	}
	return (GLint*)program_object->uniforms[location]->ivalue;
}

void enqueue_uniform_command(gl_uniform_command* cmd)
{
	if (glContext.uniform_commands == nullptr)
	{
		glContext.uniform_commands = cmd;
	}
	else
	{
		glContext.uniform_commands_tail->next = cmd;
	}
	glContext.uniform_commands_tail = cmd;
}


void gl_uniform_command::execute()
{
	switch (type)
	{
	case GL_INT:
	{
		GLint* value = gl_get_uniform_i_check(program, location);
		if (value == nullptr) return;
		value[0] = get_int(0);
		break;
	}
	case GL_INT_VEC2:
	{
		GLint* value = gl_get_uniform_i_check(program, location);
		if (value == nullptr) return;
		value[0] = get_int(0);
		value[1] = get_int(1);
		break;
	}
	case GL_INT_VEC3:
	{
		GLint* value = gl_get_uniform_i_check(program, location);
		if (value == nullptr) return;
		value[0] = get_int(0);
		value[1] = get_int(1);
		value[2] = get_int(2);
		break;
	}
	case GL_INT_VEC4:
	{
		GLint* value = gl_get_uniform_i_check(program, location);
		if (value == nullptr) return;
		value[0] = get_int(0);
		value[1] = get_int(1);
		value[2] = get_int(2);
		value[3] = get_int(3);
		break;
	}
	case GL_FLOAT:
	{
		GLfloat* value = gl_get_uniform_f_check(program, location);
		if (value == nullptr) return;
		value[0] = get_float(0);
		break;
	}
	case GL_FLOAT_VEC2:
	{
		GLfloat* value = gl_get_uniform_f_check(program, location);
		if (value == nullptr) return;
		value[0] = get_float(0);
		value[1] = get_float(1);
		break;
	}
	case GL_FLOAT_VEC3:
	{
		GLfloat* value = gl_get_uniform_f_check(program, location);
		if (value == nullptr) return;
		value[0] = get_float(0);
		value[1] = get_float(1);
		value[2] = get_float(2);
		break;
	}
	case GL_FLOAT_VEC4:
	{
		GLfloat* value = gl_get_uniform_f_check(program, location);
		if (value == nullptr) return;
		value[0] = get_float(0);
		value[1] = get_float(1);
		value[2] = get_float(2);
		value[3] = get_float(3);
		break;
	}
	case GL_FLOAT_MAT2:
	{
		GLfloat* value = gl_get_uniform_f_check(program, location);
		if (value == nullptr) return;
		value[0] = get_float(0);
		value[1] = get_float(1);
		value[2] = get_float(2);
		value[3] = get_float(3);
		break;
	}
	case GL_FLOAT_MAT3:
	{
		GLfloat* value = gl_get_uniform_f_check(program, location);
		if (value == nullptr) return;
		value[0] = get_float(0);
		value[1] = get_float(1);
		value[2] = get_float(2);
		value[3] = get_float(3);
		value[4] = get_float(4);
		value[5] = get_float(5);
		value[6] = get_float(6);
		value[7] = get_float(7);
		value[8] = get_float(8);
		break;
	}
	case GL_FLOAT_MAT4:
	{
		GLfloat* value = gl_get_uniform_f_check(program, location);
		if (value == nullptr) return;
		value[0] = get_float(0);
		value[1] = get_float(1);
		value[2] = get_float(2);
		value[3] = get_float(3);
		value[4] = get_float(4);
		value[5] = get_float(5);
		value[6] = get_float(6);
		value[7] = get_float(7);
		value[8] = get_float(8);
		value[9] = get_float(9);
		value[10] = get_float(10);
		value[11] = get_float(11);
		value[12] = get_float(12);
		value[13] = get_float(13);
		value[14] = get_float(14);
		value[15] = get_float(15);
		break;
	}
	}
}

 void glUniform1i(GLint location, GLint v0)
{
	gl_uniform_command* cmd = (gl_uniform_command*)gl_malloc(sizeof(gl_uniform_command));
	cmd->location = location;
	cmd->program = glContext.program;
	cmd->type = GL_INT;
	cmd->next = nullptr;
	cmd->set(0, v0);

	enqueue_uniform_command(cmd);
}

 void glUniform2i(GLint location, GLint v0, GLint v1)
{
	gl_uniform_command* cmd = (gl_uniform_command*)gl_malloc(sizeof(gl_uniform_command));
	cmd->location = location;
	cmd->program = glContext.program;
	cmd->type = GL_INT_VEC2;
	cmd->next = nullptr;
	cmd->set(0, v0);
	cmd->set(1, v1);

	enqueue_uniform_command(cmd);
}

 void glUniform3i(GLint location, GLint v0, GLint v1, GLint v2)
{
	gl_uniform_command* cmd = (gl_uniform_command*)gl_malloc(sizeof(gl_uniform_command));
	cmd->location = location;
	cmd->program = glContext.program;
	cmd->type = GL_INT_VEC3;
	cmd->next = nullptr;
	cmd->set(0, v0);
	cmd->set(1, v1);
	cmd->set(2, v2);

	enqueue_uniform_command(cmd);
}

 void glUniform4i(GLint location, GLint v0, GLint v1, GLint v2, GLint v3)
{
	gl_uniform_command* cmd = (gl_uniform_command*)gl_malloc(sizeof(gl_uniform_command));
	cmd->location = location;
	cmd->program = glContext.program;
	cmd->type = GL_INT_VEC4;
	cmd->next = nullptr;
	cmd->set(0, v0);
	cmd->set(1, v1);
	cmd->set(2, v2);
	cmd->set(3, v3);

	enqueue_uniform_command(cmd);
}

 void glUniform1f(GLint location, GLfloat v0)
{
	gl_uniform_command* cmd = (gl_uniform_command*)gl_malloc(sizeof(gl_uniform_command));
	cmd->location = location;
	cmd->program = glContext.program;
	cmd->type = GL_FLOAT;
	cmd->next = nullptr;
	cmd->set(0, v0);

	enqueue_uniform_command(cmd);
}

 void glUniform2f(GLint location, GLfloat v0, GLfloat v1)
{
	gl_uniform_command* cmd = (gl_uniform_command*)gl_malloc(sizeof(gl_uniform_command));
	cmd->location = location;
	cmd->program = glContext.program;
	cmd->type = GL_FLOAT_VEC2;
	cmd->next = nullptr;
	cmd->set(0, v0);
	cmd->set(1, v1);

	enqueue_uniform_command(cmd);
}

 void glUniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2)
{
	gl_uniform_command* cmd = (gl_uniform_command*)gl_malloc(sizeof(gl_uniform_command));
	cmd->location = location;
	cmd->program = glContext.program;
	cmd->type = GL_FLOAT_VEC3;
	cmd->next = nullptr;
	cmd->set(0, v0);
	cmd->set(1, v1);
	cmd->set(2, v2);
	
	enqueue_uniform_command(cmd);
}

 void glUniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
	gl_uniform_command* cmd = (gl_uniform_command*)gl_malloc(sizeof(gl_uniform_command));
	cmd->location = location;
	cmd->program = glContext.program;
	cmd->type = GL_FLOAT_VEC4;
	cmd->next = nullptr;
	cmd->set(0, v0);
	cmd->set(1, v1);
	cmd->set(2, v2);
	cmd->set(3, v3);

	enqueue_uniform_command(cmd);
}

 void glUniform1fv(GLint location, GLsizei count, const GLfloat *v)
{
	gl_uniform_command* cmd = (gl_uniform_command*)gl_malloc(sizeof(gl_uniform_command));
	cmd->location = location;
	cmd->program = glContext.program;
	cmd->type = GL_FLOAT;
	cmd->next = nullptr;
	cmd->set(0, v[0]);

	enqueue_uniform_command(cmd);
}

 void glUniform2fv(GLint location, GLsizei count, const GLfloat *v)
{
	gl_uniform_command* cmd = (gl_uniform_command*)gl_malloc(sizeof(gl_uniform_command));
	cmd->location = location;
	cmd->program = glContext.program;
	cmd->type = GL_FLOAT_VEC2;
	cmd->next = nullptr;
	cmd->set(0, v[0]);
	cmd->set(1, v[1]);

	enqueue_uniform_command(cmd);
}

 void glUniform3fv(GLint location, GLsizei count, const GLfloat *v)
{
	gl_uniform_command* cmd = (gl_uniform_command*)gl_malloc(sizeof(gl_uniform_command));
	cmd->location = location;
	cmd->program = glContext.program;
	cmd->type = GL_FLOAT_VEC3;
	cmd->next = nullptr;
	cmd->set(0, v[0]);
	cmd->set(1, v[1]);
	cmd->set(2, v[2]);

	enqueue_uniform_command(cmd);
}

 void glUniform4fv(GLint location, GLsizei count, const GLfloat *v)
{
	gl_uniform_command* cmd = (gl_uniform_command*)gl_malloc(sizeof(gl_uniform_command));
	cmd->location = location;
	cmd->program = glContext.program;
	cmd->type = GL_FLOAT_VEC4;
	cmd->next = nullptr;
	cmd->set(0, v[0]);
	cmd->set(1, v[1]);
	cmd->set(2, v[2]);
	cmd->set(3, v[3]);

	enqueue_uniform_command(cmd);
}

 void glUniform1iv(GLint location, GLsizei count, const GLint *v)
{
	gl_uniform_command* cmd = (gl_uniform_command*)gl_malloc(sizeof(gl_uniform_command));
	cmd->location = location;
	cmd->program = glContext.program;
	cmd->type = GL_INT;
	cmd->next = nullptr;
	cmd->set(0, v[0]);

	enqueue_uniform_command(cmd);
}
 void glUniform2iv(GLint location, GLsizei count, const GLint *v)
{
	gl_uniform_command* cmd = (gl_uniform_command*)gl_malloc(sizeof(gl_uniform_command));
	cmd->location = location;
	cmd->program = glContext.program;
	cmd->type = GL_INT_VEC2;
	cmd->next = nullptr;
	cmd->set(0, v[0]);
	cmd->set(1, v[1]);

	enqueue_uniform_command(cmd);
}

 void glUniform3iv(GLint location, GLsizei count, const GLint *v)
{
	gl_uniform_command* cmd = (gl_uniform_command*)gl_malloc(sizeof(gl_uniform_command));
	cmd->location = location;
	cmd->program = glContext.program;
	cmd->type = GL_INT_VEC3;
	cmd->next = nullptr;
	cmd->set(0, v[0]);
	cmd->set(1, v[1]);
	cmd->set(2, v[2]);

	enqueue_uniform_command(cmd);
}

 void glUniform4iv(GLint location, GLsizei count, const GLint *v)
{
	gl_uniform_command* cmd = (gl_uniform_command*)gl_malloc(sizeof(gl_uniform_command));
	cmd->location = location;
	cmd->program = glContext.program;
	cmd->type = GL_INT_VEC4;
	cmd->next = nullptr;
	cmd->set(0, v[0]);
	cmd->set(1, v[1]);
	cmd->set(2, v[2]);
	cmd->set(3, v[3]);

	enqueue_uniform_command(cmd);
}

 void glUnitformMatrix2fv(GLint location, GLsizei count, GLbool transpose, const GLfloat* v)
{
	gl_uniform_command* cmd = (gl_uniform_command*)gl_malloc(sizeof(gl_uniform_command));
	cmd->location = location;
	cmd->program = glContext.program;
	cmd->type = GL_FLOAT_MAT2;
	cmd->next = nullptr;
	cmd->set(0, v[0]);
	cmd->set(1, v[1]);
	cmd->set(2, v[2]);
	cmd->set(3, v[3]);

	enqueue_uniform_command(cmd);
}

 void glUnitformMatrix3fv(GLint location, GLsizei count, GLbool transpose, const GLfloat* v)
{
	gl_uniform_command* cmd = (gl_uniform_command*)gl_malloc(sizeof(gl_uniform_command));
	cmd->location = location;
	cmd->program = glContext.program;
	cmd->type = GL_FLOAT_MAT3;
	cmd->next = nullptr;
	cmd->set(0, v[0]);
	cmd->set(1, v[1]);
	cmd->set(2, v[2]);
	cmd->set(3, v[3]);
	cmd->set(4, v[4]);
	cmd->set(5, v[5]);
	cmd->set(6, v[6]);
	cmd->set(7, v[7]);
	cmd->set(8, v[8]);

	enqueue_uniform_command(cmd);
}

 void glUnitformMatrix4fv(GLint location, GLsizei count, GLbool transpose, const GLfloat* v)
{
	gl_uniform_command* cmd = (gl_uniform_command*)gl_malloc(sizeof(gl_uniform_command));
	cmd->location = location;
	cmd->program = glContext.program;
	cmd->type = GL_FLOAT_MAT4;
	cmd->next = nullptr;
	cmd->set(0, v[0]);
	cmd->set(1, v[1]);
	cmd->set(2, v[2]);
	cmd->set(3, v[3]);
	cmd->set(4, v[4]);
	cmd->set(5, v[5]);
	cmd->set(6, v[6]);
	cmd->set(7, v[7]);
	cmd->set(8, v[8]);
	cmd->set(9, v[9]);
	cmd->set(10, v[10]);
	cmd->set(11, v[11]);
	cmd->set(12, v[12]);
	cmd->set(13, v[13]);
	cmd->set(14, v[14]);
	cmd->set(15, v[15]);

	enqueue_uniform_command(cmd);
}

gl_uniform_node* gl_uniform_node::create(GLfloat* value,const GLchar* name, GLsizei size, GLenum type)
{
	gl_uniform_node* node = (gl_uniform_node*)gl_malloc(sizeof(gl_uniform_node));
	node->uniform = (gl_uniform*)gl_malloc(sizeof(gl_uniform));
	node->uniform->size = size;
	node->uniform->type = type;
	node->uniform->fvalue = value;
	node->next = nullptr;
	GLsizei buffer_size = sizeof(node->uniform->name);
	strcpy_s(node->uniform->name, buffer_size, name);
	return node;
}

gl_uniform_node* gl_uniform_node::create(GLuint* value, const GLchar* name, GLsizei size, GLenum type)
{
	gl_uniform_node* node = (gl_uniform_node*)gl_malloc(sizeof(gl_uniform_node));
	node->uniform = (gl_uniform*)gl_malloc(sizeof(gl_uniform));
	node->uniform->size = size;
	node->uniform->type = type;
	node->uniform->ivalue = value;
	node->next = nullptr;
	GLsizei buffer_size = sizeof(node->uniform->name);
	strcpy_s(node->uniform->name, buffer_size, name);
	return node;
}

const GLvoid* gl_shader::get_varing_attribute(GLsizei x, GLsizei y)
{
	return __rs->get_fragment(x, y).varing_attribute;
}
