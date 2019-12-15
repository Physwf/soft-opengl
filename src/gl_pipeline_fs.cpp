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

void gl_fragment_shader(gl_draw_command* cmd)
{
	if (cmd->vs.program == 0) return;
	gl_program_object* program_object = gl_find_program_object(cmd->vs.program);
	if (program_object == nullptr) return;
	gl_shader_object* fs_object = program_object->fragment_shader_object;
	if (fs_object == nullptr) return;
	gl_shader* fs = fs_object->shader;
	if (fs == nullptr) return;

	fs->update_screen_size(cmd->rs.buffer_width, cmd->rs.buffer_height);
	for (GLsizei y = 0; y < cmd->rs.buffer_height; ++y)
	{
		for (GLsizei x = 0; x < cmd->rs.buffer_width; ++x)
		{
			gl_fragment& fragment = cmd->rs.get_fragment(x, y);
			gl_frame_buffer* frame_buffer = glPipeline.frame_buffers[glPipeline.back_buffer_index];
			if (fragment.depth < 1.0f && fragment.depth >= 0.0f && fragment.depth < frame_buffer->get_depth(x,y))
			{
				GLvoid* fs_out = fs->fs_process((GLbyte*)fragment.varing_attribute, &cmd->rs, x,y);
				gl_vector4& color = ((gl_vector4*)fs_out)[0];
				frame_buffer->set_color(x, y, color);
				frame_buffer->set_depth(x, y, fragment.depth);
			}
		}
	}
}
