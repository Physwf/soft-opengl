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

#include "gl_memory.h"
#include "gl_utilities.h"
#include "gl_objects.h"

struct gl_buffer_object : public gl_named_object
{
	GLvoid* data;
	GLsizei size;
};

gl_buffer_object* gl_find_buffer_object(GLuint name);
void gl_allocate_buffer(GLuint buffer, GLsizei size,GLvoid* data);
void gl_realse_buffer(GLuint buffer);
