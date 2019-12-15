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

#include <cassert>
#include <memory>

void gl_point_rasterize(gl_draw_command* cmd)
{
	gl_primitive_node* node = cmd->pa.primitives;
	while (node)
	{
		gl_vector4* position = (gl_vector4*)node->vertices;
		position->x /= position->w;
		position->y /= position->w;
		position->z /= position->w;
		GLsizei x = (GLsizei)((position->x+1.0f)/2.0f * cmd->pa.viewport_width);
		GLsizei y = (GLsizei)((position->y+1.0f)/2.0f * cmd->pa.viewport_height);
		if (x >= 0 && x < cmd->pa.viewport_width && y >= 0 && y < cmd->pa.viewport_height)
		{
			gl_fragment& fragment = cmd->rs.fragment_buffer[y*cmd->pa.viewport_width + x];
			GLfloat depth = gl_campf(position->z);
			if (fragment.depth > position->z)
			{
				fragment.destroy();
				assert(depth >= 0.0f && depth <= 1.0f);
				fragment.depth = depth;
				fragment.varing_attribute = gl_malloc(cmd->pa.vertex_size);
				memcpy_s(fragment.varing_attribute, cmd->pa.vertex_size, node->vertices, cmd->pa.vertex_size);
			}
		}
		node = node->next;
	}
}
//diamond exit algorithm
bool gl_is_in_diamond(const gl_vector2& p1, const gl_vector2& p2, GLsizei xw, GLsizei yw, bool bxmajor)
{
	gl_vector2 left(xw - 0.5f, (GLfloat)yw);
	gl_vector2 right(xw + 0.5f, (GLfloat)yw);
	gl_vector2 top((GLfloat)xw, yw + 0.5f);
	gl_vector2 down((GLfloat)xw, yw - 0.5f);
	if (bxmajor)
	{
		if (gl_is_line_segment_intersect(left, down, p1, p2) || gl_is_line_segment_intersect(right, down, p1, p2))//同下半段相交
		{
			return true;
		}
		if (gl_is_line_segment_intersect(left, top, p1, p2))//同左上相交
		{
			if (!gl_is_point_in_line_segment(left, top, p1) && !gl_is_point_in_line_segment(left, top, p2)) return true;//且不在线段上
		}
		if (gl_is_line_segment_intersect(right, top, p1, p2))//同右上相交
		{
			if (!gl_is_point_in_line_segment(right, top, p1) && !gl_is_point_in_line_segment(right, top, p2)) return true;//且不在线段上
		}
	}
	else
	{
		if (gl_is_line_segment_intersect(left, top, p1, p2) || gl_is_line_segment_intersect(right, top, p1, p2))//同上半段相交
		{
			return true;
		}
		if (gl_is_line_segment_intersect(left, down, p1, p2))//同左下相交
		{
			if (!gl_is_point_in_line_segment(left, down, p1) && !gl_is_point_in_line_segment(left, down, p2)) return true;//且不在线段上
		}
		if (gl_is_line_segment_intersect(right, down, p1, p2))//同右下相交
		{
			if (!gl_is_point_in_line_segment(right, down, p1) && !gl_is_point_in_line_segment(right, down, p2)) return true;//且不在线段上
		}
	}
	return false;
}

void gl_line_list_rasterize(gl_draw_command* cmd)
{
	gl_primitive_node* node = cmd->pa.primitives;
	while (node)
	{
		gl_vector4* p1 = (gl_vector4*)node->vertices;
		gl_vector4* p2 = (gl_vector4*)((GLbyte*)node->vertices + cmd->pa.vertex_size);
		p1->x /= p1->w;
		p1->y /= p1->w;
		p1->z /= p1->w;
		p2->x /= p2->w;
		p2->y /= p2->w;
		p2->z /= p2->w;
		assert(p2->w != 0.0f && p1->w != 0.0f);
		GLfloat x1 = ((p1->x + 1.0f) * cmd->pa.viewport_width) / 2.0f;
		GLfloat y1 = ((p1->y + 1.0f) * cmd->pa.viewport_height) / 2.0f;
		GLfloat x2 = ((p2->x + 1.0f) * cmd->pa.viewport_width) / 2.0f;
		GLfloat y2 = ((p2->y + 1.0f) * cmd->pa.viewport_height) / 2.0f;
		GLfloat k = (p2->y - p1->y) / (p2->x - p1->x);
		GLsizei xstart, ystart, xend, yend;
		xstart = (GLsizei)glMin(x1, x2)-1;
		ystart = (GLsizei)glMin(y1, y2)-1;
		xend = (GLsizei)glMax(x1, x2)+1;
		yend = (GLsizei)glMax(y1, y2)+1;
		xstart = glClamp(xstart, 0, cmd->pa.viewport_width);
		ystart = glClamp(xstart, 0, cmd->pa.viewport_height);
		xend = glClamp(xend, 0, cmd->pa.viewport_width);
		yend = glClamp(yend, 0, cmd->pa.viewport_height);
		bool bxmajor = k >= -1.0f && k <= 1.0f;
		for (GLsizei y = ystart; y != yend; ++y)
		{
			for (GLsizei x = xstart; x != xend; ++x)
			{
				if (gl_is_in_diamond(gl_vector2(x1, y1), gl_vector2(x2, y2), x, y, bxmajor))
				{
					gl_fragment& fragment = cmd->rs.get_fragment(x, y);
					GLfloat t = bxmajor ? (x - x1) / (x2 - x1) : (y - y1) / (y2 - y1);
					t = gl_campf(t);
					GLfloat depth = gl_lerp(p1->z, p2->z, t);
					if (depth > 0.0f && fragment.depth > depth)
					{
						fragment.destroy();
						fragment.depth = depth;
						fragment.varing_attribute = gl_lerp(p1, p2, cmd->pa.vertex_size / sizeof(gl_vector4), t);
					}
				}
			}
		}
		node = node->next;
	}
}

void gl_line_list_adj_rasterize(gl_draw_command* cmd)
{

}

void gl_line_strip_rasterize(gl_draw_command* cmd)
{

}

void gl_line_strip_adj_rasterize(gl_draw_command* cmd)
{

}

bool is_top_left_edge(const gl_vector4* start, const gl_vector4* end)
{
	return (end->x < start->x) || (end->y > start->y);
}

GLfloat gl_barycentric_interpolation(GLfloat v1, GLfloat v2, GLfloat v3, GLfloat m2, GLfloat m3)
{
	return v1 + m2 * (v2 - v1) + m3 * (v3 - v1);
}

gl_vector4 gl_barycentric_interpolation(const gl_vector4& p1, const gl_vector4& p2, const gl_vector4& p3, GLfloat m2, GLfloat m3)
{
	gl_vector4 result;
	result.x = gl_barycentric_interpolation(p1.x, p2.x, p3.x, m2, m3);
	result.y = gl_barycentric_interpolation(p1.y, p2.y, p3.y, m2, m3);
	result.z = gl_barycentric_interpolation(p1.z, p2.z, p3.z, m2, m3);
	result.w = gl_barycentric_interpolation(p1.w, p2.w, p3.w, m2, m3);
	return result;
}

gl_vector4* gl_barycentric_interpolation(const gl_vector4* p1, const gl_vector4* p2, const gl_vector4* p3, GLsizei n, GLfloat m2, GLfloat m3)
{
	gl_vector4* result = (gl_vector4*)gl_malloc(n * sizeof(gl_vector4),"gl_barycentric_interpolation");
	for (GLsizei i = 0; i < n; ++i)
	{
		*(result + i) = gl_barycentric_interpolation(p1[i],p2[i],p3[i],m2,m3);
	}
	return result;
}

void gl_do_triangle_aabb_rasterize(gl_primitive_node* node,GLsizei vertex_size,GLsizei viewport_width,GLsizei viewport_height, gl_rs_state& rs)
{
	gl_vector4* p1 = (gl_vector4*)node->vertices;
	gl_vector4* p2 = (gl_vector4*)((GLbyte*)node->vertices + vertex_size);
	gl_vector4* p3 = (gl_vector4*)((GLbyte*)node->vertices + vertex_size * 2);
	p1->x /= p1->w;
	p1->y /= p1->w;
	p1->z /= p1->w;
	p2->x /= p2->w;
	p2->y /= p2->w;
	p2->z /= p2->w;
	p3->x /= p3->w;
	p3->y /= p3->w;
	p3->z /= p3->w;
	assert(p2->w != 0.0f && p1->w != 0.0f && p3->w != 0.0f);
	GLfloat x1 = (p1->x + 1.0f) * viewport_width / 2.0f;
	GLfloat y1 = (p1->y + 1.0f) * viewport_height / 2.0f;
	GLfloat x2 = (p2->x + 1.0f) * viewport_width / 2.0f;
	GLfloat y2 = (p2->y + 1.0f) * viewport_height / 2.0f;
	GLfloat x3 = (p3->x + 1.0f) * viewport_width / 2.0f;
	GLfloat y3 = (p3->y + 1.0f) * viewport_height / 2.0f;
	GLsizei xstart, ystart, xend, yend;
	xstart = (GLsizei)glMin(x1, x2, x3)-1;
	ystart = (GLsizei)glMin(y1, y2, y3)-1;
	xend = (GLsizei)glMax(x1, x2, x3)+1;
	yend = (GLsizei)glMax(y1, y2, y3)+1;
	xstart = glClamp(xstart, 0, viewport_width);
	ystart = glClamp(ystart, 0, viewport_height);
	xend = glClamp(xend, 0, viewport_width);
	yend = glClamp(yend, 0, viewport_height);
	gl_vector2 v1 = p1->to_vector2();
	gl_vector2 v2 = p2->to_vector2();
	gl_vector2 v3 = p3->to_vector2();

	GLfloat double_area = get_triangle_orientation(v1,v2,v3);

// 	GLfloat bias1 = is_top_left_edge(p2, p3) ? 0.0f : (rs.cull_mode == GL_CW ? 1.0f : -1.0f);
// 	GLfloat bias2 = is_top_left_edge(p3, p1) ? 0.0f : (rs.cull_mode == GL_CW ? 1.0f : -1.0f);
// 	GLfloat bias3 = is_top_left_edge(p1, p2) ? 0.0f : (rs.cull_mode == GL_CW ? 1.0f : -1.0f);
	GLfloat bias1 =  0.0f;
	GLfloat bias2 =  0.0f;
	GLfloat bias3 =  0.0f;
	for (GLsizei y = ystart; y != yend; ++y)
	{
		for (GLsizei x = xstart; x != xend; ++x)
		{
			
			gl_vector2 p((x * 2.0f / viewport_width) - 1.0f, (y* 2.0f / viewport_height) - 1.0f);
			GLfloat w1 = get_triangle_orientation(v2, v3, p);
			GLfloat w2 = get_triangle_orientation(v3, v1, p);
			GLfloat w3 = get_triangle_orientation(v1, v2, p);
			//if(y<yend-1) continue;
			bool is_in_triangle = (rs.cull_mode == GL_CW) ? ((w1 + bias1) >= 0.0f && (w2 + bias2) >= 0.0f && (w3 + bias3) >= 0.0f) : ((w1 + bias1) <= 0.0f && (w2 + bias2) <= 0.0f && (w3 + bias3) <= 0.0f);
			if (is_in_triangle)
			{
				GLfloat m2 = w2 / double_area;
				GLfloat m3 = w3 / double_area;
				GLfloat depth = 1.0f - gl_barycentric_interpolation(p1->z, p2->z, p3->z, m2, m3);
				gl_fragment& fragment = rs.get_fragment(x, y);
				if (depth > 0.0f && fragment.depth > depth)
				{
					fragment.destroy();
					fragment.depth = depth;
					fragment.varing_attribute = gl_barycentric_interpolation(p1, p2, p3, vertex_size / sizeof(gl_vector4), m2, m3);
				}
			}
		}
	}
}

void gl_triangle_list_rasterize(gl_draw_command* cmd)
{
	gl_primitive_node* node = cmd->pa.primitives;
	int i = 0;
	while (node)
	{
		++i;
		gl_do_triangle_aabb_rasterize(node,cmd->pa.vertex_size, cmd->pa.viewport_width,cmd->pa.viewport_height,cmd->rs);
		node = node->next;
	}
}

void gl_triangle_list_adj_rasterize(gl_draw_command* cmd)
{

}

void gl_triangle_strip_rasterize(gl_draw_command* cmd)
{

}

void gl_triangle_strip_adj_rasterize(gl_draw_command* cmd)
{

}

void gl_rasterize(gl_draw_command* cmd)
{
	cmd->rs.clear_fragment_buffer();
	switch (cmd->ia.primitive_type)
	{
	case GL_POINT_LIST:
		gl_point_rasterize(cmd);
		break;
	case GL_LINE_LIST:
		gl_line_list_rasterize(cmd);
		break;
	case GL_LINE_LIST_ADJ:
		gl_line_list_adj_rasterize(cmd);
		break;
	case GL_LINE_STRIP:
		gl_line_strip_rasterize(cmd);
		break;
	case GL_LINE_STRIP_ADJ:
		gl_line_strip_adj_rasterize(cmd);
		break;
	case GL_TRIANGLE_LIST:
		gl_triangle_list_rasterize(cmd);
		break;
	case GL_TRIANGLE_LIST_ADJ:
		gl_triangle_list_adj_rasterize(cmd);
		break;
	case GL_TRIANGLE_STRIP:
		gl_triangle_strip_rasterize(cmd);
		break;
	case GL_TRIANGLE_STRIP_ADJ:
		gl_triangle_strip_adj_rasterize(cmd);
		break;
	}
}
