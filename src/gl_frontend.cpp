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
#include "gl_utilities.h"
#include "gl_memory.h"
#include "gl_pipeline.h"

#include <assert.h>
#include <string.h>

gl_frontend glContext;

void glContextInit()
{
	glClearError();

	glContext.viewport_x = 0;
	glContext.viewport_y = 0;
	glContext.viewport_width = 256; //glClamp(width, (GLuint)MIN_BUFFER_WIDTH, (GLuint)MAX_BUFFER_WIDTH);
	glContext.viewport_height = 256;// glClamp(width, (GLuint)MIN_BUFFER_HEIGHT, (GLuint)MAX_BUFFER_HEIGHT);

	glContext.depth_near = 0.0f;
	glContext.depth_far = 1.0f;

	glContext.array_buffer = 0;
	glContext.index_buffer = 0;

	glContext.indices_pointer = nullptr;

	glContext.program = 0;
	glContext.uniform_commands = nullptr;

	glContext.front_face = GL_CCW;
	glContext.cull_face = GL_BACK;


	for (gl_atrribute<GLfloat>& attrib : glContext.vertex_attributes)
	{
		attrib.values[0] = attrib.values[1] = attrib.values[2] = 0.0f;
		attrib.values[3] = 1.0f;
	}

	for (gl_atrribute_pointer& attrib_ptr : glContext.vertex_attribute_pointers)
	{
		attrib_ptr.bEnabled = false;
		attrib_ptr.pointer = nullptr;
		attrib_ptr.size = 0;
		attrib_ptr.type = 0;
		attrib_ptr.stride = 0;
	}

	glContext.unpack_mode = UNPACK_ALIGNMENT_4;
	glContext.selected_texture_unit = &glContext.texture_params[0];

	gl_pipeline_init();
}

void glContextDestroy()
{
	glClearError();
}

void glClearError()
{
	glContext.error = GL_NO_ERROR;
	glContext.error_desc[0] = 0;
}

void glSetError(GLenum error, const GLchar* szErrorDesc)
{
	glContext.error = error;
	assert(strlen(szErrorDesc) < MAX_ERROR_DESC_LEGHT);
	strcpy_s(glContext.error_desc, szErrorDesc);
}

 void glViewport(GLint x, GLint y, GLsizei w, GLsizei h)
{
	glClearError();
	if (w <= 0 || h <= 0)
	{
		glSetError(GL_INVALID_VALUE, "viewport width and height must be greater than 0!");
		return;
	}
	glContext.viewport_width = w;
	glContext.viewport_height = h;
	glContext.viewport_x = x;
	glContext.viewport_y = y;
}

 void glDepthRangef(GLclampf n, GLclampf f)
{
	glContext.depth_near = glClamp(n, 0.0f, 1.0f);
	glContext.depth_far = glClamp(f, 0.0f, 1.0f);
}

 void glClearColor(GLclampf r, GLclampf g, GLclampf b, GLclampf a)
{
	glClearError();

	glContext.clear_color.r = glClamp(r, 0.0f, 1.0f);
	glContext.clear_color.g = glClamp(g, 0.0f, 1.0f);
	glContext.clear_color.b = glClamp(b, 0.0f, 1.0f);
	glContext.clear_color.a = glClamp(a, 0.0f, 1.0f);
}

 void glClearDepth(GLclampf d)
{
	glClearError();
	glContext.clear_depth = glClamp(d, 0.0f, 1.0f);
}

 void glClear(GLbitfield buf)
{
	glContext.clear_bitmask = buf;
	gl_do_clear();
}

 void glFrontFace(GLenum dir)
{
	glContext.front_face = dir;
}

 void glCullFace(GLenum mode)
{
	glContext.cull_face = mode;
}

 void glFlush()
{
	gl_do_draw();
}

 void glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
	glClearError();

	if (count < 0)
	{
		glSetError(GL_INVALID_VALUE, "count can't be less than zero!");
		return;
	}

	switch (mode)
	{
	case GL_POINT_LIST:
	case GL_LINE_LIST:
	case GL_LINE_LIST_ADJ:
	case GL_LINE_STRIP:
	case GL_LINE_STRIP_ADJ:
	case GL_TRIANGLE_LIST:
	case GL_TRIANGLE_LIST_ADJ:
	case GL_TRIANGLE_STRIP:
	case GL_TRIANGLE_STRIP_ADJ:
		glContext.draw_mode = mode;
		glContext.indices_pointer = nullptr;
		glContext.indices_type = GL_SHORT;
		glContext.count = count;
		break;
	default:
		glSetError(GL_INVALID_ENUM, "Illegal mode argument!");
		return;
		break;
	}
	gl_emit_draw_command();
}

 void glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices)
{
	glClearError();

	switch (mode)
	{
	case GL_POINT_LIST:
	case GL_LINE_LIST:
	case GL_LINE_LIST_ADJ:
	case GL_LINE_STRIP:
	case GL_LINE_STRIP_ADJ:
	case GL_TRIANGLE_LIST:
	case GL_TRIANGLE_LIST_ADJ:
	case GL_TRIANGLE_STRIP:
	case GL_TRIANGLE_STRIP_ADJ:
	{
		glContext.draw_mode = mode;
		glContext.count = count;
		glContext.indices_pointer = indices;
		glContext.indices_type = type;

		switch (type)
		{
		case GL_BYTE:
		case GL_SHORT:
			break;
		default:
			glSetError(GL_INVALID_ENUM, "Illegal type argument!");
		}

	}
	break;
	default:
		glSetError(GL_INVALID_ENUM, "Illegal mode argument!");
		return;
		break;
	}
	gl_emit_draw_command();
}

