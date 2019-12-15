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
#include "gl_objects.h"
#include "gl_utilities.h"

struct gl_texture2d_mipmap
{
	GLsizei width;
	GLsizei height;
	GLfloat* data;

	GLfloat* get_data(GLsizei x, GLsizei y)
	{
		x = glClamp(x, 0, width);
		y = glClamp(y, 0, height);
		return data + ((y*width + x) * 4);
	}
};

struct gl_texture2d
{
	GLenum format;
	GLsizei mipmap_count;
	gl_texture2d_mipmap** mipmaps;
};

struct gl_texture_cube
{
	GLenum format;
	GLsizei mipmap_count;
	gl_texture2d_mipmap** mipmaps_x_positive;
	gl_texture2d_mipmap** mipmaps_x_nagetive;
	gl_texture2d_mipmap** mipmaps_y_positive;
	gl_texture2d_mipmap** mipmaps_y_nagetive;
	gl_texture2d_mipmap** mipmaps_z_positive;
	gl_texture2d_mipmap** mipmaps_z_nagetive;
};

struct gl_texture_object : public gl_named_object
{
	GLenum type;
	GLvoid* texture;
};

struct gl_texture_target
{
	GLint wrap_mode_s;
	GLint wrap_mode_t;
	GLint min_filter;
	GLint mag_filter;
	gl_texture_object* binded_object;
};

struct gl_texture_unit_params
{
	gl_texture_target texture2d_target;
	gl_texture_target texture_cube_target;
};

struct gl_texture_unit
{
	gl_texture_unit_params params;
};

gl_texture_object*		gl_find_texture_object(GLuint name);
gl_texture2d*			gl_create_texture2d();
gl_texture_cube*		gl_create_texture_cube();
void					gl_sample_texture2d(GLuint index, GLfloat l, GLfloat s, GLfloat t, GLfloat* result);
void					gl_sample_texture_cube(GLuint index, GLfloat s, GLfloat t, GLfloat u, GLfloat* result);
