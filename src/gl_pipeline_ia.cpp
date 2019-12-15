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

#include "gl_pipeline.h"

#include <memory>

GLsizei get_vertex_count_from_indices(gl_draw_command* cmd)
{
	GLsizei num_vertex = 0;
	if (cmd->ia.indices)
	{
		GLshort* indices = (GLshort*)cmd->ia.indices;
		for (GLsizei i = 0; i < cmd->ia.indices_count; ++i)
		{
			GLshort current_index = cmd->ia.indices[i] + 1;
			if (num_vertex < current_index)
			{
				num_vertex = current_index;
			}
		}
	}
	return num_vertex;
}

template <typename T>
gl_vector4 to_vector4_from_attribute(const GLvoid* pointer, GLsizei index, GLsizei stride, GLsizei size)
{
	T* start = (T*)pointer + index * (stride / sizeof(T));
	gl_vector4 result(0.0f, 0.0f, 0.0f, 1.0f);
	result.set(start, size);
	return result;
}

gl_vector4 get_attribute_from_attribute_pointer(const gl_atrribute_pointer& attr_pointer, GLsizei index)
{
	switch (attr_pointer.type)
	{
	case GL_BYTE:
		return to_vector4_from_attribute<GLbyte>(attr_pointer.pointer, index, attr_pointer.stride, attr_pointer.size);
	case GL_UNSIGNED_BYTE:
		return to_vector4_from_attribute<GLubyte>(attr_pointer.pointer, index, attr_pointer.stride, attr_pointer.size);
	case GL_SHORT:
		return to_vector4_from_attribute<GLshort>(attr_pointer.pointer, index, attr_pointer.stride, attr_pointer.size);
	case GL_UNSIGNED_SHORT:
		return to_vector4_from_attribute<GLushort>(attr_pointer.pointer, index, attr_pointer.stride, attr_pointer.size);
	case GL_INT:
		return to_vector4_from_attribute<GLint>(attr_pointer.pointer, index, attr_pointer.stride, attr_pointer.size);
	case GL_UNSIGNED_INT:
		return to_vector4_from_attribute<GLuint>(attr_pointer.pointer, index, attr_pointer.stride, attr_pointer.size);
	case GL_FLOAT:
		return to_vector4_from_attribute<GLfloat>(attr_pointer.pointer, index, attr_pointer.stride, attr_pointer.size);
	case GL_DOUBLE:
		return to_vector4_from_attribute<GLdouble>(attr_pointer.pointer, index, attr_pointer.stride, attr_pointer.size);
	default:
		return gl_vector4(0.0f, 0.0f, 0.0f, 1.0f);
	}
}

bool gl_check_input_validity(gl_draw_command* cmd)
{
	//check validity(todo)
	switch (cmd->ia.primitive_type)
	{
	case GL_POINT_LIST:
		break;
	case GL_LINE_LIST:
		break;
	case GL_LINE_LIST_ADJ:
		break;
	case GL_LINE_STRIP:
		break;
	case GL_LINE_STRIP_ADJ:
		break;
	case GL_TRIANGLE_LIST:
		break;
	case GL_TRIANGLE_LIST_ADJ:
		break;
	case GL_TRIANGLE_STRIP:
		break;
	case GL_TRIANGLE_STRIP_ADJ:
		break;
	}
	return true;
}

template<typename TIndex>
void gl_fill_indices_copy(gl_draw_command* cmd, const TIndex* index_data, GLsizei count)
{
	GLsizei indices_size = count * sizeof(GLshort);
	cmd->ia.indices_count = count;

	cmd->ia.indices = (GLshort*)gl_malloc(indices_size);

	if (index_data == nullptr)
	{
		for (GLsizei i = 0; i < count; ++i)
		{
			cmd->ia.indices[i] = i;
		}
	}
	else
	{
		for (GLsizei i = 0; i < count; ++i)
		{
			cmd->ia.indices[i] = index_data[i];
		}
	}
}

void gl_input_assemble(gl_draw_command* cmd)
{
	GLsizei vertex_size = cmd->vs.vertex_size;

	//assemble indices
	switch (cmd->ia.indices_type)
	{
	case GL_BYTE:
		gl_fill_indices_copy<GLbyte>(cmd, (const GLbyte*)cmd->ia.indices_copy, cmd->ia.indices_count);
		break;
	case GL_SHORT:
		gl_fill_indices_copy<GLshort>(cmd, (const GLshort*)cmd->ia.indices_copy, cmd->ia.indices_count);
		break;
	default:
		return;
	}
	//assemble vertices
	GLsizei vertex_count = get_vertex_count_from_indices(cmd);
	cmd->ia.vertices_count = vertex_count;
	cmd->ia.vertices = gl_malloc(vertex_size*vertex_count);
	gl_vector4* vertices = (gl_vector4*)cmd->ia.vertices;

	// If an array corresponding to a generic attribute required by a vertex shader is not enabled, 
	// then the corresponding element is taken from the current generic attribute state	// OpenGLR ES Common Profile Specification Version 2.0.25 (Full Specification) (November 2, 2010)
	// 顶点属性取决于vertex shader
	GLsizei attrib_count = vertex_size / sizeof(gl_vector4);
	for (int v = 0; v < vertex_count; ++v)
	{
		for (int i = 0; i < attrib_count; ++i)
		{
			if (cmd->ia.vertex_attribute_pointers[i].bEnabled)
			{
				gl_vector4& attribute = vertices[v*attrib_count + i];
				attribute = get_attribute_from_attribute_pointer(cmd->ia.vertex_attribute_pointers[i], v);
			}
			else
			{
				gl_vector4& attribute = vertices[v*attrib_count + i];
				attribute.x = cmd->ia.vertex_attributes[i].x;
				attribute.y = cmd->ia.vertex_attributes[i].y;
				attribute.z = cmd->ia.vertex_attributes[i].z;
				attribute.w = cmd->ia.vertex_attributes[i].w;
			}
		}
	}

	if (!gl_check_input_validity(cmd)) return;
}