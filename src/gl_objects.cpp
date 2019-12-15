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
#include "gl_objects.h"
#include "gl_shader.h"
#include "gl_buffer.h"
#include "gl_texture.h"
#include "gl_pipeline.h"

GLuint gl_create_named_object(GLenum type)
{
	GLsizei object_size = 0;
	gl_named_object* object = nullptr;
	switch (type)
	{
	case __VERTEX_SHADER_OBJECT__:
	case __FRAGMENT_SHADER_OBJECT__:
	{
		gl_shader_object* shader_object = (gl_shader_object*)gl_malloc(sizeof(gl_shader_object));
		shader_object->shader = nullptr;
		object = shader_object;
	}
	break;
	case __PROGRAM_OBJECT__:
	{
		gl_program_object* program_object = (gl_program_object*)gl_malloc(sizeof(gl_program_object));
		for (gl_uniform*& uniform : program_object->uniforms)
		{
			uniform = nullptr;
		}
		program_object->vertex_shader_object = nullptr;
		program_object->fragment_shader_object = nullptr;
		object = program_object;
	}
	break;
	case __BUFFER_OBJECT__:
	{
		gl_buffer_object* buffer_object = (gl_buffer_object*)gl_malloc(sizeof(gl_buffer_object));
		buffer_object->data = nullptr;
		buffer_object->size = 0;
		object = buffer_object;
		break;
	}
	case __TEXTURE_OBJECT__:
	{
		gl_texture_object* texture_object = (gl_texture_object*)gl_malloc(sizeof(gl_texture_object));
		texture_object->texture = nullptr;
		texture_object->type = 0;
		object = texture_object;
		break;
	}
	default:
		glSetError(GL_INVALID_ENUM, "Invalid shader type!");
		return 0;
	}

	gl_named_object_node* node = glPipeline.named_object_list;
	while (node && node->next)
	{
		node = node->next;
	}
	gl_named_object_node* new_node = (gl_named_object_node*)gl_malloc(sizeof(gl_named_object_node));
	if (node == nullptr)
	{
		node = new_node;
		object->name = 1;
		glPipeline.named_object_list = node;
	}
	else
	{
		node->next = new_node;
		object->name = node->object->name + 1;
	}
	object->type = type;
	new_node->object = object;
	new_node->next = nullptr;
	return object->name;
}

gl_named_object_node* gl_find_named_object(GLuint name)
{
	gl_named_object_node* node = glPipeline.named_object_list;
	while (node)
	{
		if (node->object->name == name) return node;
		node = node->next;
	}
	return nullptr;
}

