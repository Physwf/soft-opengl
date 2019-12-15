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

#include "gl_buffer.h"
#include "gl_frontend.h"

#include <memory>


gl_buffer_object* gl_find_buffer_object(GLuint name)
{
	gl_named_object_node* node = gl_find_named_object(name);
	if (node == nullptr) return nullptr;
	gl_named_object* object = node->object;
	if (object == nullptr) return nullptr;
	if (object->type != __BUFFER_OBJECT__)
	{
		return nullptr;
	}
	return (gl_buffer_object*)object;
}

void gl_allocate_buffer(GLuint buffer, GLsizei size, GLvoid* data)
{
	gl_buffer_object* object = gl_find_buffer_object(buffer);
	if (object)
	{
		object->data = gl_malloc(size);
		object->size = size;
		memcpy_s(object->data, size, data, size);
		return;
	}
	glSetError(GL_INVALID_OPERATION, "illegal buffer!");
}

void gl_realse_buffer(GLuint buffer)
{
	gl_buffer_object* object = gl_find_buffer_object(buffer);
	if (object)
	{
		if (object->size != 0)
		{
			gl_free(object->data);
			object->data = nullptr;
			object->size = 0;
			return;
		}
	}
	glSetError(GL_INVALID_OPERATION, "illegal buffer!");
}


 void glGenBuffer(GLsizei n, GLuint* buffers)
{
	GLsizei i = 0;
	while (i < n)
	{
		buffers[i++] = gl_create_named_object(__BUFFER_OBJECT__);
	}
}

 void glBindBuffer(GLenum target, GLuint buffer)
{

}

 void glBufferData(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage)
{

}

 void glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data)
{

}

 void glGenVertexArrays(GLsizei n, GLuint* buffers)
{

}
 void glDeleteVertexArrays(GLsizei n, GLuint* buffers)
{

}
 void glBindVertexArray(GLuint buffer)
{

}