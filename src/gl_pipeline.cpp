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
#include "gl_shader.h"

#include <cassert>
#include <memory>

gl_vector2 operator+(const gl_vector2& lhs, const gl_vector2& rhs)
{
	return gl_vector2(lhs.x + rhs.x, lhs.y + rhs.y);
}

gl_vector2 operator-(const gl_vector2& lhs, const gl_vector2& rhs)
{
	return gl_vector2(lhs.x - rhs.x, lhs.y - rhs.y);
}

float operator*(const gl_vector2& lhs, const gl_vector2& rhs)
{
	return (lhs.x * rhs.x + lhs.y * rhs.y);
}

gl_vector4 operator*(float value, const gl_vector4& rhs)
{
	return gl_vector4(rhs.x * value, rhs.y * value, rhs.z * value, rhs.w * value);
}

gl_vector4 operator+(const gl_vector4& lhs, const gl_vector4& rhs)
{
	return gl_vector4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w);
}

gl_vector4 operator-(const gl_vector4& lhs, const gl_vector4& rhs)
{
	return gl_vector4(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w);
}

gl_vector2 operator/(const gl_vector2& lhs, float divider)
{
	return { lhs.x / divider,lhs.y / divider };
}

float cross(const gl_vector2& lhs, const gl_vector2& rhs)
{
	return (lhs.x* rhs.y - rhs.x*lhs.y);
}

gl_vector3 operator+(const gl_vector3& lhs, const gl_vector3& rhs)
{
	return gl_vector3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}
gl_vector3 operator-(const gl_vector3& lhs, const gl_vector3& rhs)
{
	return gl_vector3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.y + rhs.y);
}
float operator*(const gl_vector3& lhs, const gl_vector3& rhs)
{
	return (lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z);
}

gl_pipeline glPipeline;

bool gl_pipeline_init()
{
	glPipeline.command_list = nullptr;
	glPipeline.command_tail = nullptr;

	glPipeline.ring_buffer.buffer_head = nullptr;
	glPipeline.ring_buffer.buffer_tail = nullptr;

	for (int i = 0; i < 3; ++i)
	{
		glPipeline.frame_buffers[i] = nullptr;// gl_frame_buffer::create(512, 512);
	}
	glPipeline.front_buffer_index = -1;
	glPipeline.back_buffer_index = 0;

	glPipeline.named_object_list = nullptr;


	for (int i = 0; i < 3; ++i)
	{
		if (glPipeline.frame_buffers[i] == nullptr)
		{
			glPipeline.frame_buffers[i] = gl_frame_buffer::create(glContext.viewport_width, glContext.viewport_height);
			glPipeline.frame_buffers[i]->buffer_width = glContext.viewport_width;
			glPipeline.frame_buffers[i]->buffer_height = glContext.viewport_height;
			glPipeline.front_buffer_index = -1;
			glPipeline.back_buffer_index = 0;
		}
	}
	return true;
}

void gl_emit_draw_command()
{
	gl_draw_command* cmd = glPipeline.ring_buffer.allocate();
	//ia
	if (glContext.indices_pointer)
	{
		GLsizei indices_size = glContext.count*(glContext.indices_type == GL_BYTE ? 1 : 2);
		cmd->ia.indices_copy = gl_malloc(indices_size);
		memcpy_s(cmd->ia.indices_copy, indices_size, glContext.indices_pointer, indices_size);
		cmd->ia.indices_count = glContext.count;
	}
	else
	{
		cmd->ia.indices = nullptr;
		cmd->ia.indices_copy = nullptr;
		cmd->ia.indices_count = glContext.count;
	}
	cmd->ia.indices = nullptr;
	cmd->ia.primitive_type = glContext.draw_mode;
	cmd->ia.vertices = nullptr;
	cmd->ia.indices_type = glContext.indices_type;
	memcpy_s(cmd->ia.vertex_attributes, sizeof(cmd->ia.vertex_attributes), glContext.vertex_attributes, sizeof(glContext.vertex_attributes));
	memcpy_s(cmd->ia.vertex_attribute_pointers, sizeof(cmd->ia.vertex_attribute_pointers), glContext.vertex_attribute_pointers, sizeof(glContext.vertex_attribute_pointers));
	//vs
	cmd->vs.program = glContext.program;
	cmd->vs.vertices_result = nullptr;
	gl_program_object* program_object = gl_find_program_object(glContext.program);
	cmd->vs.vertex_size = program_object->vertex_shader_object->shader->input_size;
	//pa
	cmd->pa.primitive_type = cmd->ia.primitive_type;
	cmd->pa.primitives = nullptr;
	cmd->pa.tail = nullptr;
	cmd->pa.primitive_count = 0;
	cmd->pa.viewport_x = glContext.viewport_x;
	cmd->pa.viewport_y = glContext.viewport_y;
	cmd->pa.viewport_width = glContext.viewport_width;
	cmd->pa.viewport_height = glContext.viewport_height;
	cmd->pa.depth_far = glContext.depth_far;
	cmd->pa.depth_near = glContext.depth_near;
	cmd->pa.front_face = glContext.front_face;
	cmd->pa.cull_face = glContext.cull_face;
	//rs
	GLenum front_face = glContext.front_face;
	GLenum back_face = glContext.front_face == GL_CCW ? GL_CW : GL_CCW;
	cmd->rs.cull_mode = glContext.cull_face == GL_FRONT ? front_face : back_face;//剔除规则
	cmd->rs.point_size = 1.0f;
	GLsizei count = glContext.viewport_width * glContext.viewport_height;
	cmd->rs.fragment_buffer = (gl_fragment*)gl_malloc(glContext.viewport_width * glContext.viewport_height * sizeof(gl_fragment));
	assert(cmd->rs.fragment_buffer != nullptr);
	cmd->rs.buffer_width = glContext.viewport_width;
	cmd->rs.buffer_height = glContext.viewport_height;
	cmd->rs.clear_fragment_buffer();

	for (int i = 0; i < 3; ++i)
	{
		if (glPipeline.frame_buffers[i]->buffer_width != glContext.viewport_width || glPipeline.frame_buffers[i]->buffer_height != glContext.viewport_height)
		{
			gl_frame_buffer::destory(glPipeline.frame_buffers[i]);
			glPipeline.frame_buffers[i] = gl_frame_buffer::create(glContext.viewport_width, glContext.viewport_height);
			glPipeline.frame_buffers[i]->buffer_width = glContext.viewport_width;
			glPipeline.frame_buffers[i]->buffer_height = glContext.viewport_height;
			glPipeline.front_buffer_index = -1;
			glPipeline.back_buffer_index = 0;
		}
	}

	for (int i=0;i<MAX_COMBINED_TEXTURE_IMAGE_UNITS;++i)
	{
		cmd->texture_params[i] = glContext.texture_params[i];
	}

	cmd->uniform_commands = glContext.uniform_commands;
	glContext.uniform_commands = nullptr;
	//todo multi-thread
	glPipeline.enqueue(cmd);
}

void gl_do_clear()
{
	if (glContext.clear_bitmask & GL_COLOR_BUFFER_BIT)
	{
		glPipeline.frame_buffers[glPipeline.back_buffer_index]->set_clearcolor(gl_vector4(glContext.clear_color.r, glContext.clear_color.g, glContext.clear_color.b, glContext.clear_color.a));
	}
	if (glContext.clear_bitmask & GL_DEPTH_BUFFER_BIT)
	{
		glPipeline.frame_buffers[glPipeline.back_buffer_index]->set_cleardepth(glContext.clear_depth);
	}
	if (glContext.clear_bitmask & GL_STENCIL_BUFFER_BIT)
	{

	}
}

void gl_do_uniform_command(gl_draw_command* cmd)
{
	gl_uniform_command* ucmd = cmd->uniform_commands;
	while (ucmd)
	{
		ucmd->execute();
		ucmd = ucmd->next;
	}
}

void gl_do_draw()
{
	while (gl_draw_command* cmd = glPipeline.dequeue())
	{
		for (int i = 0; i < MAX_COMBINED_TEXTURE_IMAGE_UNITS; ++i)
		{
			glPipeline.texture_units[i].params = cmd->texture_params[i];
		}

		gl_do_uniform_command(cmd);
		gl_input_assemble(cmd);
		gl_vertex_shader(cmd);
		gl_primitive_assemble(cmd);
		gl_rasterize(cmd);
		gl_fragment_shader(cmd);
		gl_output_merge(cmd);
		//回收
		glPipeline.ring_buffer.deallocate(cmd);
	}
}

void gl_swap_frame_buffer()
{
	glPipeline.front_buffer_index = glPipeline.back_buffer_index;
	glPipeline.back_buffer_index++;
	glPipeline.back_buffer_index %= 3;
}

void gl_set_frame_buffer(GLsizei x, GLsizei y, const gl_vector4& color, GLfloat depth)
{
	gl_frame_buffer* buffer = glPipeline.frame_buffers[glPipeline.back_buffer_index];
	buffer->set_color(x, y, color);
	buffer->set_depth(x, y, depth);
}

SGL_API void gl_copy_front_buffer(unsigned char* rgbbuffer)
{
	gl_frame_buffer* fb = glPipeline.frame_buffers[glPipeline.front_buffer_index];
	for (GLsizei y = 0; y < fb->buffer_height; ++y)
	{
		for (GLsizei x = 0; x < fb->buffer_width; ++x)
		{
			gl_vector4 color = fb->get_color(x, y);
			GLsizei rgb_index = (y*fb->buffer_width + x) * 4;
			rgbbuffer[rgb_index + 0] = (unsigned char)(gl_campf(color.b) * 255);
			rgbbuffer[rgb_index + 1] = (unsigned char)(gl_campf(color.g) * 255);
			rgbbuffer[rgb_index + 2] = (unsigned char)(gl_campf(color.r) * 255);
		}
	}

// 	for (LONG y = 0; y < Height; ++y)
// 	{
// 		for (LONG x = 0; x < Width; ++x)
// 		{
// 			pFrameBuffer[4 * y * Width + x * 4 + 0] = 255;
// 			pFrameBuffer[4 * y * Width + x * 4 + 1] = 0;
// 			pFrameBuffer[4 * y * Width + x * 4 + 2] = 0;
// 		}
// 	}
}
