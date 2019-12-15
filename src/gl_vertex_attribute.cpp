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

#include "gl.h"
#include "gl_frontend.h"
#include "gl_utilities.h"
#include "gl_memory.h"
#include <memory>


SGL_API void glVertexAttrib1f(GLuint index, GLfloat x)
{
	glClearError();
	if (index > MAX_VERTEX_ATTRIBUTE)
	{
		glSetError(GL_INVALID_VALUE, "index exceed maximum value allowed!");
		return;
	}
	glContext.vertex_attributes[index] = gl_atrribute<GLfloat>(x, 0, 0, 1);
}

SGL_API void glVertexAttrib2f(GLuint index, GLfloat x, GLfloat y)
{
	glClearError();
	if (index > MAX_VERTEX_ATTRIBUTE)
	{
		glSetError(GL_INVALID_VALUE, "index exceed maximum value allowed!");
		return;
	}
	glContext.vertex_attributes[index] = gl_atrribute<GLfloat>(x, y, 0, 1);
}

SGL_API void glVertexAttrib3f(GLuint index, GLfloat x, GLfloat y, GLfloat z)
{
	glClearError();
	if (index > MAX_VERTEX_ATTRIBUTE)
	{
		glSetError(GL_INVALID_VALUE, "index exceed maximum value allowed!");
		return;
	}
	glContext.vertex_attributes[index] = gl_atrribute<GLfloat>(x, y, z, 1);
}

SGL_API void glVertexAttrib4f(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
	glClearError();
	if (index > MAX_VERTEX_ATTRIBUTE)
	{
		glSetError(GL_INVALID_VALUE, "index exceed maximum value allowed!");
		return;
	}
	glContext.vertex_attributes[index] = gl_atrribute<GLfloat>(x, y, z, w);
}

SGL_API void glVertexAttrib1fv(GLuint index, const GLfloat* values)
{
	glClearError();
	if (index > MAX_VERTEX_ATTRIBUTE)
	{
		glSetError(GL_INVALID_VALUE, "index exceed maximum value allowed!");
		return;
	}
	glContext.vertex_attributes[index] = gl_atrribute<GLfloat>(values[0], 0, 0, 1);
}

SGL_API void glVertexAttrib2fv(GLuint index, const GLfloat* values)
{
	glClearError();
	if (index > MAX_VERTEX_ATTRIBUTE)
	{
		glSetError(GL_INVALID_VALUE, "index exceed maximum value allowed!");
		return;
	}
	glContext.vertex_attributes[index] = gl_atrribute<GLfloat>(values[0], values[1], 0, 1);
}

SGL_API void glVertexAttrib3fv(GLuint index, const GLfloat* values)
{
	glClearError();
	if (index > MAX_VERTEX_ATTRIBUTE)
	{
		glSetError(GL_INVALID_VALUE, "index exceed maximum value allowed!");
		return;
	}
	glContext.vertex_attributes[index] = gl_atrribute<GLfloat>(values[0], values[1], values[2], 1);
}

SGL_API void glVertexAttrib4fv(GLuint index, const GLfloat* values)
{
	glClearError();
	if (index > MAX_VERTEX_ATTRIBUTE)
	{
		glSetError(GL_INVALID_VALUE, "index exceed maximum value allowed!");
		return;
	}
	glContext.vertex_attributes[index] = gl_atrribute<GLfloat>(values[0], values[1], values[2], values[3]);
}

SGL_API void glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLbool normalized, GLsizei stride, const void* ptr)
{
	glClearError();
	if (index > MAX_VERTEX_ATTRIBUTE)
	{
		glSetError(GL_INVALID_VALUE, "index exceed maximum value allowed!");
		return;
	}
	glContext.vertex_attribute_pointers[index].pointer = ptr;
	glContext.vertex_attribute_pointers[index].stride = stride;
	glContext.vertex_attribute_pointers[index].type = type;
	glContext.vertex_attribute_pointers[index].size = size;
}

SGL_API void glEnableVertexAttribArray(GLuint index)
{
	glClearError();
	if (index > MAX_VERTEX_ATTRIBUTE)
	{
		glSetError(GL_INVALID_VALUE, "index exceed maximum value allowed!");
		return;
	}
	glContext.vertex_attribute_pointers[index].bEnabled = true;
}


SGL_API void glDisableVertexAttribArray(GLuint index)
{
	glClearError();
	if (index > MAX_VERTEX_ATTRIBUTE)
	{
		glSetError(GL_INVALID_VALUE, "index exceed maximum value allowed!");
		return;
	}
	glContext.vertex_attribute_pointers[index].bEnabled = false;
}




