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

#include <algorithm>
#include <iterator>
#include <vector>
#include <set>
#include <cassert>
#include <memory>

bool gl_is_point_culled(const gl_vector4* p)
{
	if (p->x > p->w || p->x < -p->w) return true;
	if (p->y > p->w || p->y < -p->w) return true;
	if (p->z > p->w || p->z < 0.0f) return true;
	return false;
}

//https://en.wikipedia.org/wiki/Cohen-Sutherland_algorithm
bool gl_is_line_culled(const gl_vector4* p1, const gl_vector4* p2, bool& baccepted, bool& bculled1, bool& bculled2)
{
	GLbyte code1 = 0;
	if (p1->x < -p1->w)	code1 |= 1;
	if (p1->x > p1->w)	code1 |= 2;
	if (p1->y < -p1->w)	code1 |= 4;
	if (p1->y > p1->w)	code1 |= 8;
	bculled1 = (code1 != 0);
	GLbyte code2 = 0;
	if (p2->x < -p2->w)	code2 |= 1;
	if (p2->x > p2->w)	code2 |= 2;
	if (p2->y < -p2->w)	code2 |= 4;
	if (p2->y > p2->w)	code2 |= 8;
	bculled2 = (code2 != 0);
	baccepted = (code1 | code2) == 0;
	return (code1 & code2) != 0;
}

bool gl_is_triangle_culled(const gl_vector4* p1, const gl_vector4* p2, const gl_vector4* p3, bool& baccepted, bool& bculled1, bool& bculled2, bool& bculled3)
{
	GLbyte code1 = 0;
	if (p1->x < -p1->w)	code1 |= 1;
	if (p1->x > p1->w)	code1 |= 2;
	if (p1->y < -p1->w)	code1 |= 4;
	if (p1->y > p1->w)	code1 |= 8;
	bculled1 = (code1 != 0);
	GLbyte code2 = 0;
	if (p2->x < -p2->w)	code2 |= 1;
	if (p2->x > p2->w)	code2 |= 2;
	if (p2->y < -p2->w)	code2 |= 4;
	if (p2->y > p2->w)	code2 |= 8;
	bculled1 = (code2 != 0);
	GLbyte code3 = 0;
	if (p3->x < -p3->w)	code3 |= 1;
	if (p3->x > p3->w)	code3 |= 2;
	if (p3->y < -p3->w)	code3 |= 4;
	if (p2->y > p3->w)	code3 |= 8;
	bculled3 = (code3 != 0);
	baccepted = (code1 | code2 | code3) == 0;
	return (code1 & code2 & code3) != 0;
}

bool gl_is_face_culled(const gl_vector4* p1, const gl_vector4* p2, const gl_vector4* p3,GLenum cull_face, GLenum front_face)
{
	if (cull_face == GL_FRONT_AND_BACK) return true;
	
	bool is_ccw = get_triangle_orientation(p1->to_vector2(), p2->to_vector2(), p3->to_vector2()) > 0.0f;
	switch (front_face)
	{
	case GL_CCW:
		if (cull_face == GL_FRONT)
		{
			return is_ccw;
		}
		else
		{
			return !is_ccw;
		}
		break;
	case GL_CW:
		if (cull_face == GL_FRONT)
		{
			return !is_ccw;
		}
		else
		{
			return is_ccw;
		}
		break;
	}
	return false;
}

bool gl_is_point_degenerated(gl_vector4* p)
{
	return (p->x == 0 && p->y == 0 && p->z == 0 && p->w == 0);
}

gl_primitive_node* gl_do_line_clipping(GLvoid* v1, GLvoid* v2, bool bculled1, bool bculled2, GLsizei vertex_size)
{
	gl_vector4* p1 = (gl_vector4*)v1;
	gl_vector4* p2 = (gl_vector4*)v2;
	assert(p1->w != 0 && p2->w != 0);
	GLfloat ts[4] = { 0.0f };
	if (p1->x == p2->x)//垂直
	{
		ts[0] = (p1->y - p1->w) / (p1->y - p2->y);
		ts[1] = (p1->y - (-p1->w)) / (p1->y - p2->y);
	}
	else if (p1->y == p2->y)//水平
	{
		ts[2] = (p1->x - p1->w) / (p1->x - p2->x);
		ts[3] = (p1->x - (-p1->w)) / (p1->x - p2->x);
	}
	else
	{
		ts[0] = (p1->y - p1->w) / (p1->y - p2->y);
		ts[1] = (p1->y - (-p1->w)) / (p1->y - p2->y);
		ts[2] = (p1->x - p1->w) / (p1->x - p2->x);
		ts[3] = (p1->x - (-p1->w)) / (p1->x - p2->x);
	}
	gl_vector4 pts[5] =
	{
		gl_vector4(0.0f, 0.0f, 0.0f, 1.0f) ,
		gl_vector4(0.0f, 0.0f, 0.0f, 1.0f) ,
		gl_vector4(0.0f, 0.0f, 0.0f, 1.0f) ,
		gl_vector4(0.0f, 0.0f, 0.0f, 1.0f) ,
		*p2,
	};
	std::sort(std::begin(ts), std::end(ts));
	int count = 0;
	for (int i = 0; i < 4; ++i)
	{
		if (ts[i] <= 0) continue;
		if (i > 0 && ts[i - 1] == ts[i])
		{
			continue;
		}
		ts[count++] = ts[i];
	}
	for (auto i = 0; i < count; ++i)
	{
		GLfloat t = ts[i];
		if (t <= 0.0f) continue;
		pts[i].w = p1->w * t + (1 - t) * p2->w;
		pts[i].x = p1->x * t + (1 - t) * p2->x;
		pts[i].y = p1->y * t + (1 - t) * p2->y;
		pts[i].z = p1->z * t + (1 - t) * p2->z;
	}
	gl_vector4* p = p1;
	GLfloat t = 0.0f;
	for (auto i = 0; i < 5; ++i)
	{
		bool bacceped = false;
		bool bculled1 = false;
		bool bculled2 = false;
		gl_vector4* pp = &pts[i];
		GLfloat tt = ts[i];
		if (gl_is_point_degenerated(pp)) continue;//skip
		if (gl_is_line_culled(p, pp, bacceped, bculled1, bculled2))
		{
			p = pp;
			t = tt;
			continue;
		}
		else
		{
			//baccepted must be true
			gl_primitive_node* node = (gl_primitive_node*)gl_malloc(sizeof(gl_primitive_node));
			GLsizei primitive_size = vertex_size * 2;
			node->vertices = gl_malloc(primitive_size);
			node->next = nullptr;
			node->vertices_count = 2;
			if (p == p1)
			{
				memcpy_s(node->vertices, vertex_size, p1, vertex_size);//just copy
				gl_vector4 pt(0.0f, 0.0f, 0.0f, 0.0f);
				for (GLsizei i = 0; i < vertex_size; i += sizeof(gl_vector4))//do lerp
				{
					gl_vector4* p1i = p1 + i / sizeof(gl_vector4);
					gl_vector4* p2i = p2 + i / sizeof(gl_vector4);
					pt.w = p1i->w * tt + (1 - tt) * p2i->w;
					pt.x = p1i->x * tt + (1 - tt) * p2i->x;
					pt.y = p1i->y * tt + (1 - tt) * p2i->y;
					pt.z = p1i->z * tt + (1 - tt) * p2i->z;
					if (pt.w == 0.0f)
					{
						pt.w = 1.0f;
					}
					assert(pt.w != 0.0f);
					memcpy_s((gl_vector4*)((GLbyte*)node->vertices + vertex_size) + i / sizeof(gl_vector4), sizeof(gl_vector4), &pt, sizeof(gl_vector4));
				}
			}
			else
			{
				if (pp = p2)
				{
					gl_vector4 pt(0.0f, 0.0f, 0.0f, 0.0f);
					for (GLsizei i = 0; i < vertex_size; i += sizeof(gl_vector4))//do lerp
					{
						gl_vector4* p1i = p1 + i / sizeof(gl_vector4);
						gl_vector4* p2i = p2 + i / sizeof(gl_vector4);
						pt.w = p1i->w * t + (1 - t) * p2i->w;
						pt.x = p1i->x * t + (1 - t) * p2i->x;
						pt.y = p1i->y * t + (1 - t) * p2i->y;
						pt.z = p1i->z * t + (1 - t) * p2i->z;
						assert(pt.w != 0.0f);
						memcpy_s((gl_vector4*)node->vertices + i / sizeof(gl_vector4), sizeof(gl_vector4), &pt, sizeof(gl_vector4));
					}
					memcpy_s((GLbyte*)node->vertices + vertex_size, vertex_size, p2, vertex_size);//just copy
				}
				else
				{
					//do lerp
					gl_vector4 pt(0.0f, 0.0f, 0.0f, 0.0f);
					for (GLsizei i = 0; i < vertex_size; i += sizeof(gl_vector4))
					{
						gl_vector4* p1i = p1 + i / sizeof(gl_vector4);
						gl_vector4* p2i = p2 + i / sizeof(gl_vector4);
						pt.w = p1i->w * t + (1 - t) * p2i->w;
						pt.x = p1i->x * t + (1 - t) * p2i->x;
						pt.y = p1i->y * t + (1 - t) * p2i->y;
						pt.z = p1i->z * t + (1 - t) * p2i->z;
						assert(pt.w != 0.0f);
						memcpy_s((gl_vector4*)node->vertices + i / sizeof(gl_vector4), sizeof(gl_vector4), &pt, sizeof(gl_vector4));
					}
					for (GLsizei i = 0; i < vertex_size; i += sizeof(gl_vector4))
					{
						gl_vector4* p1i = p1 + i / sizeof(gl_vector4);
						gl_vector4* p2i = p2 + i / sizeof(gl_vector4);
						pt.w = p1i->w * tt + (1 - tt) * p2i->w;
						pt.x = p1i->x * tt + (1 - tt) * p2i->x;
						pt.y = p1i->y * tt + (1 - tt) * p2i->y;
						pt.z = p1i->z * tt + (1 - tt) * p2i->z;
						assert(pt.w != 0.0f);
						memcpy_s((gl_vector4*)((GLbyte*)node->vertices + vertex_size) + i / sizeof(gl_vector4), sizeof(gl_vector4), &pt, sizeof(gl_vector4));
					}
				}
			}
			return node;//only one node can be produced, so just return
		}
	}
	return nullptr;
}
//clockwise inside means in right side
bool is_in_right_side_of_boundary(const gl_vector2& cw_boundary_start, const gl_vector2& cw_boundary_end, const gl_vector2& point)
{
	return cross(cw_boundary_end - cw_boundary_start, point - cw_boundary_start) >= 0;//==0表示在边界上
}
const gl_vector4* compute_intersection(const gl_vector4* cur_p,const gl_vector4* pre_p, const gl_vector2& cw_boundary_start, const gl_vector2& cw_boundary_end, GLsizei vertex_size)
{
	GLfloat t = 0.0f;
	if (cw_boundary_start.x == cw_boundary_end.x)
	{
		if (cw_boundary_start.x == -1.0f)
		{
			t = (cur_p->x - (-1.0f)) / (cur_p->x - pre_p->x);
		}
		else if (cw_boundary_start.x == 1.0f)
		{
			t = (cur_p->x - 1.0f) / (cur_p->x - pre_p->x);
		}
	}
	else if (cw_boundary_start.y == cw_boundary_end.y)
	{
		if (cw_boundary_start.y == -1.0f)
		{
			t = (cur_p->y - (-1.0f)) / (cur_p->y - pre_p->y);
		}
		else if (cw_boundary_start.y == 1.0f)
		{
			t = (cur_p->y - 1.0f) / (cur_p->y - pre_p->y);
		}
	}

	//if (t <= 0.0f || t >= 1.0f) return nullptr;

	gl_vector4* result = (gl_vector4*)gl_malloc(vertex_size);
	gl_vector4 pt(0.0f, 0.0f, 0.0f, 0.0f);
	for (GLsizei i = 0; i < vertex_size / (GLsizei)sizeof(gl_vector4); ++i )
	{
		const gl_vector4* pre_pi = (pre_p + i);
		const gl_vector4* cur_pi = (cur_p + i);
		pt.w = cur_pi->w * (1 - t) + t * pre_pi->w;
		pt.x = cur_pi->x * (1 - t) + t * pre_pi->x;
		pt.y = cur_pi->y * (1 - t) + t * pre_pi->y;
		pt.z = cur_pi->z * (1 - t) + t * pre_pi->z;
		memcpy_s(result + i, sizeof(gl_vector4), &pt, sizeof(gl_vector4));
	}
	return result;
}
///https://en.wikipedia.org/wiki/Sutherland-Hodgman_algorithm
enum gl_frustum_edge
{
	top,
	right,
	bottom,
	left
};
void do_sutherland_hodgman_clipping(const std::vector<const gl_vector4*>& vertices_in, std::vector<const gl_vector4*>& vertices_out, gl_frustum_edge edge, GLsizei vertex_size)
{
	vertices_out.clear();
	gl_vector2 clockwise_pstart;
	gl_vector2 clockwise_pend;
	switch (edge)
	{
	case top:
		clockwise_pstart = gl_vector2(-1.0f, 1.0f);
		clockwise_pend = gl_vector2(1.0f, 1.0f);
		break;
	case right:
		clockwise_pstart = gl_vector2(1.0f, 1.0f);
		clockwise_pend = gl_vector2(1.0f, -1.0f);
		break;
	case bottom:
		clockwise_pstart = gl_vector2(1.0f, -1.0f);
		clockwise_pend = gl_vector2(-1.0f, -1.0f);
		break;
	case left:
		clockwise_pstart = gl_vector2(-1.0f, -1.0f);
		clockwise_pend = gl_vector2(-1.0f, 1.0f);
		break;
	}
	for (std::vector<gl_vector4*>::size_type i = 0; i < vertices_in.size(); ++i)
	{
		const gl_vector4* cur_p = vertices_in[i];
		const gl_vector4* pre_p = vertices_in[(i + vertices_in.size() - 1) % vertices_in.size()];
		const gl_vector4* intersectin_p = compute_intersection(cur_p, pre_p, clockwise_pstart, clockwise_pend, vertex_size);
		if (is_in_right_side_of_boundary(clockwise_pend, clockwise_pstart, cur_p->to_vector2() / cur_p->w))
		{
			if (!is_in_right_side_of_boundary(clockwise_pend, clockwise_pstart, pre_p->to_vector2() / cur_p->w))
			{
				if(intersectin_p != nullptr) vertices_out.push_back(intersectin_p);
			}
			vertices_out.push_back(cur_p);
		}
		else if (is_in_right_side_of_boundary(clockwise_pend, clockwise_pstart, pre_p->to_vector2() / cur_p->w))
		{
			if (intersectin_p != nullptr) vertices_out.push_back(intersectin_p);
		}
	}
}


gl_primitive_node* gl_do_triangle_clipping(const GLvoid* v1, const GLvoid* v2, const GLvoid* v3, GLsizei vertex_size, gl_pa_state& pa)
{
	const gl_vector4* p1 = (gl_vector4*)v1;
	const gl_vector4* p2 = (gl_vector4*)v2;
	const gl_vector4* p3 = (gl_vector4*)v3;

	std::vector<const gl_vector4*> vertices_in = { p1, p2, p3 };
	std::vector<const gl_vector4*> vertices_out;
	std::set<const gl_vector4*> all_vertices;
	do_sutherland_hodgman_clipping(vertices_in, vertices_out, gl_frustum_edge::top, vertex_size);
	all_vertices.insert(vertices_in.begin(), vertices_in.end());
	do_sutherland_hodgman_clipping(vertices_out, vertices_in, gl_frustum_edge::right, vertex_size);
	all_vertices.insert(vertices_out.begin(), vertices_out.end());
	do_sutherland_hodgman_clipping(vertices_in, vertices_out, gl_frustum_edge::bottom, vertex_size);
	all_vertices.insert(vertices_in.begin(), vertices_in.end());
	do_sutherland_hodgman_clipping(vertices_out, vertices_in, gl_frustum_edge::left, vertex_size);
	//内存回收
	
	if (vertices_in.size() > 0)
	{
		assert(vertices_in.size() >= 3);
		for (std::vector<const gl_vector4*>::size_type i = 0; i < vertices_in.size() - 2; i++)
		{
			gl_primitive_node* node = (gl_primitive_node*)gl_malloc(sizeof(gl_primitive_node));
			node->next = nullptr;
			node->vertices = gl_malloc(vertex_size * 3);
			node->vertices_count = 3;
			const GLvoid* vertices[3] = { vertices_in[0], vertices_in[i + 1], vertices_in[i + 2] };
			for (auto j = 0; j < 3; ++j)
			{
				memcpy_s((GLbyte*)node->vertices + j * vertex_size, vertex_size, vertices[j], vertex_size);
			}
			if (pa.tail == nullptr)
			{
				pa.primitives = node;
			}
			else
			{
				pa.tail->next = node;
			}
			pa.tail = node;
			++pa.primitive_count;
		}
	}

	for (const gl_vector4* v : all_vertices)
	{
		if (v == p1 || v == p2 || v == p3) continue;
		gl_free((GLvoid*)v);
	}
	return nullptr;
}

void gl_point_assemble(gl_draw_command* cmd)
{
	gl_primitive_node* tail = nullptr;
	for (GLsizei i = 0; i < cmd->ia.vertices_count; ++i)
	{
		const GLbyte* vertex_data = (GLbyte*)cmd->vs.vertices_result + i * cmd->pa.vertex_size;
		gl_vector4* position = (gl_vector4*)(vertex_data);
		if (gl_is_point_culled(position)) continue;
		gl_primitive_node* node = (gl_primitive_node*)gl_malloc(sizeof(gl_primitive_node));
		node->vertices = gl_malloc(cmd->pa.vertex_size);
		memcpy_s(node->vertices, cmd->pa.vertex_size, vertex_data, cmd->pa.vertex_size);
		node->next = nullptr;
		node->vertices_count = 1;
		if (tail == nullptr)
		{
			cmd->pa.primitives = node;
		}
		else
		{
			tail->next = node;
		}
		tail = node;
		++cmd->pa.primitive_count;
	}
}

void gl_line_list_assemble(gl_draw_command* cmd)
{
	for (GLsizei i = 0; i < cmd->ia.vertices_count; i += 2)
	{
		GLbyte* vertex_data1 = (GLbyte*)cmd->vs.vertices_result + (i + 0) * cmd->pa.vertex_size;
		GLbyte* vertex_data2 = (GLbyte*)cmd->vs.vertices_result + (i + 1) * cmd->pa.vertex_size;
		gl_vector4* position1 = (gl_vector4*)(vertex_data1);
		gl_vector4* position2 = (gl_vector4*)(vertex_data2);
		if (position2->w == 0.0f)
		{
			position2->w = 1.0f;
		}
		bool baccepted = false;
		bool bculled1 = false;
		bool bculled2 = false;
		bool bculled = gl_is_line_culled(position1, position2, baccepted, bculled1, bculled2);
		if (bculled)
		{
			printf("bculled");
			continue;//cliped
		}
		else if (baccepted)
		{
			//todo z culling
			gl_primitive_node* node = (gl_primitive_node*)gl_malloc(sizeof(gl_primitive_node));
			GLsizei primitive_size = cmd->pa.vertex_size * 2;
			node->vertices = gl_malloc(primitive_size);
			memcpy_s(node->vertices, primitive_size, vertex_data1, primitive_size);
			node->next = nullptr;
			node->vertices_count = 2;
			if (cmd->pa.tail == nullptr)
			{
				cmd->pa.primitives = node;
			}
			else
			{
				cmd->pa.tail->next = node;
			}
			cmd->pa.tail = node;
			++cmd->pa.primitive_count;
		}
		else
		{
			gl_primitive_node* node = gl_do_line_clipping(vertex_data1, vertex_data2, bculled1, bculled2, cmd->pa.vertex_size);
			if (cmd->pa.tail == nullptr)
			{
				cmd->pa.primitives = node;
			}
			else
			{
				cmd->pa.tail->next = node;
			}
			cmd->pa.tail = node;
			++cmd->pa.primitive_count;
		}
	}
}

void gl_line_list_adj_assemble(gl_draw_command* cmd) { }

void gl_line_strip_assemble(gl_draw_command* cmd)
{
	for (GLsizei i = 0; i < cmd->ia.vertices_count - 1; ++i)
	{
		GLbyte* vertex_data1 = (GLbyte*)cmd->vs.vertices_result + cmd->pa.vertex_size;
		GLbyte* vertex_data2 = (GLbyte*)cmd->vs.vertices_result + (i + 1) * cmd->pa.vertex_size;
		gl_vector4* position1 = (gl_vector4*)(vertex_data1);
		gl_vector4* position2 = (gl_vector4*)(vertex_data2);
		bool baccepted = false;
		bool bculled1 = false;
		bool bculled2 = false;
		if (gl_is_line_culled(position1, position2, baccepted, bculled1, bculled2))
		{
			continue;
		}
		else if (baccepted)
		{
			//todo z culling
			gl_primitive_node* node = (gl_primitive_node*)gl_malloc(sizeof(gl_primitive_node));
			GLsizei primitive_size = cmd->pa.vertex_size * 2;
			node->vertices = gl_malloc(primitive_size);
			memcpy_s(node->vertices, primitive_size, vertex_data1, primitive_size);
			node->next = nullptr;
			node->vertices_count = 2;
			if (cmd->pa.tail == nullptr)
			{
				cmd->pa.primitives = node;
			}
			else
			{
				cmd->pa.tail->next = node;
			}
			cmd->pa.tail = node;
			++cmd->pa.primitive_count;
		}
		else
		{
			if (gl_primitive_node* node = gl_do_line_clipping(vertex_data1, vertex_data2, bculled1, bculled2, cmd->pa.vertex_size))
			{
				if (cmd->pa.tail == nullptr)
				{
					cmd->pa.primitives = node;
				}
				else
				{
					cmd->pa.tail->next = node;
				}
				cmd->pa.tail = node;
				++cmd->pa.primitive_count;
			}
		}
	}
}

void gl_line_strip_adj_assemble(gl_draw_command* cmd) { }

void gl_triangle_list_assemble(gl_draw_command* cmd)
{
	for (GLsizei i = 0; i < cmd->ia.vertices_count; i+=3)
	{
		const GLbyte* vertex_data1 = (GLbyte*)cmd->vs.vertices_result + (i + 0) * cmd->pa.vertex_size;
		const GLbyte* vertex_data2 = (GLbyte*)cmd->vs.vertices_result + (i + 1) * cmd->pa.vertex_size;
		const GLbyte* vertex_data3 = (GLbyte*)cmd->vs.vertices_result + (i + 2) * cmd->pa.vertex_size;
		gl_vector4* position1 = (gl_vector4*)(vertex_data1);
		gl_vector4* position2 = (gl_vector4*)(vertex_data2);
		gl_vector4* position3 = (gl_vector4*)(vertex_data3);
		bool baccepted = false;
		bool bculled1 = false;
		bool bculled2 = false;
		bool bculled3 = false;
		if (gl_is_face_culled(position1, position2, position3, cmd->pa.cull_face, cmd->pa.front_face))
		{
			continue;
		}
		else if (gl_is_triangle_culled(position1, position2, position3, baccepted, bculled1, bculled2, bculled3))
		{
			continue;
		}
		else if (baccepted)
		{
			gl_primitive_node* node = (gl_primitive_node*)gl_malloc(sizeof(gl_primitive_node));
			GLsizei primitive_size = cmd->pa.vertex_size * 3;
			node->vertices = gl_malloc(primitive_size);
			node->vertices_count = 3;
			memcpy_s(node->vertices, primitive_size, vertex_data1, primitive_size);
			node->next = nullptr;
			if (cmd->pa.tail == nullptr)
			{
				cmd->pa.primitives = node;
			}
			else
			{
				cmd->pa.tail->next = node;
			}
			cmd->pa.tail = node;
			++cmd->pa.primitive_count;
		}
		else
		{
			gl_do_triangle_clipping(vertex_data1, vertex_data2, vertex_data3, cmd->pa.vertex_size, cmd->pa);
		}
	}
}

void gl_triangle_list_adj_assemble(gl_draw_command* cmd) { }

void gl_triangle_strip_assemble(gl_draw_command* cmd)
{

}

void gl_triangle_strip_adj_assemble(gl_draw_command* cmd) { }

void gl_primitive_assemble(gl_draw_command* cmd)
{
	switch (cmd->ia.primitive_type)
	{
	case GL_POINT_LIST:
		gl_point_assemble(cmd);
		break;
	case GL_LINE_LIST:
		gl_line_list_assemble(cmd);
		break;
	case GL_LINE_LIST_ADJ:
		gl_line_list_adj_assemble(cmd);
		break;
	case GL_LINE_STRIP:
		gl_line_strip_assemble(cmd);
		break;
	case GL_LINE_STRIP_ADJ:
		gl_line_strip_adj_assemble(cmd);
		break;
	case GL_TRIANGLE_LIST:
		gl_triangle_list_assemble(cmd);
		break;
	case GL_TRIANGLE_LIST_ADJ:
		gl_triangle_list_adj_assemble(cmd);
		break;
	case GL_TRIANGLE_STRIP:
		gl_triangle_strip_assemble(cmd);
		break;
	case GL_TRIANGLE_STRIP_ADJ:
		gl_triangle_strip_adj_assemble(cmd);
		break;
	}
}
