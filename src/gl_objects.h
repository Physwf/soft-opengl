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

#define __VERTEX_SHADER_OBJECT__		0x10000001
#define __FRAGMENT_SHADER_OBJECT__		0x10000002
#define __PROGRAM_OBJECT__				0x10000003
#define __BUFFER_OBJECT__				0x10000004
#define __TEXTURE_OBJECT__				0x10000005

struct gl_named_object
{
	GLuint name;
	GLenum type;
};

struct gl_named_object_node
{
	gl_named_object*		object;
	gl_named_object_node*	next;
};

GLuint gl_create_named_object(GLenum type);
gl_named_object_node* gl_find_named_object(GLuint name);
void gl_destroy_named_object(GLuint name);