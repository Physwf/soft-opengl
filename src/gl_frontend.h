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

#pragma once

#include "gl.h"
#include "gl_texture.h"

#define MAX_VERTEX_ATTRIBUTE 8

#define MIN_BUFFER_WIDTH	128
#define MIN_BUFFER_HEIGHT	128
#define MAX_BUFFER_WIDTH	1920
#define MAX_BUFFER_HEIGHT	1080

#define MAX_ERROR_DESC_LEGHT 1024

#define UNPACK_ALIGNMENT_1 1
#define UNPACK_ALIGNMENT_2 2
#define UNPACK_ALIGNMENT_4 4
#define UNPACK_ALIGNMENT_8 8

#define MAX_COMBINED_TEXTURE_IMAGE_UNITS 8
#define MAX_TEXTURE_SIZE 2048
#define MAX_MIPMAP_LEVEL 11

//static_assert((1<< MAX_MIPMAP_LEVEL) == MAX_TEXTURE_SIZE);

#define CONDITION_VALIDATE(Condition,ErrorType,ErrorDesc) if(Condition) { glSetError(ErrorType,ErrorDesc); return; }

template <typename T>
union gl_atrribute
{
	typename T Type;

	union
	{
		T values[4];
		struct 
		{
			T x, y, z, w;
		};
		struct
		{
			T r, g, b, a;
		};
	};

	gl_atrribute() :values{0,0,0,0} {}
	gl_atrribute(T _x, T _y, T _z, T _w) :values{ _x,_y,_z,_w } {}
};

struct gl_atrribute_pointer
{
	GLbool	bEnabled;
	const GLvoid* pointer;
	GLint size;
	GLenum	type;
	GLsizei stride;
	gl_atrribute_pointer() :pointer(nullptr) {}
};

struct gl_vertex
{
	gl_atrribute<GLfloat> attributes[MAX_VERTEX_ATTRIBUTE];
};


struct gl_frontend
{
	GLint			viewport_x;
	GLint			viewport_y;
	GLsizei			viewport_width;
	GLsizei			viewport_height;

	GLclampf		depth_near;
	GLclampf		depth_far;
	GLbitfield		clear_bitmask;

	GLenum			front_face;
	GLenum			cull_face;

	gl_texture_unit_params	texture_params[MAX_COMBINED_TEXTURE_IMAGE_UNITS];
	gl_texture_unit_params* selected_texture_unit;//当前选中的纹理单元

	gl_atrribute<GLfloat>	clear_color;
	GLclampf				clear_depth;

	GLuint			array_buffer;
	GLuint			index_buffer;

	gl_atrribute<GLfloat>			vertex_attributes[MAX_VERTEX_ATTRIBUTE];
	gl_atrribute_pointer			vertex_attribute_pointers[MAX_VERTEX_ATTRIBUTE];

	const GLvoid* indices_pointer;
	GLenum indices_type;
	GLsizei count;
	GLenum	draw_mode;

	GLuint program;

	struct gl_uniform_command* uniform_commands;
	struct gl_uniform_command* uniform_commands_tail;

	GLint  unpack_mode;

	GLenum error;
	GLchar error_desc[MAX_ERROR_DESC_LEGHT];
};



extern gl_frontend glContext;

void glContextInit();
void glContextDestroy();
void glClearError();
void glSetError(GLenum error, const GLchar* szErrorDesc);