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
#include "gl_texture.h"
#include "gl_frontend.h"
#include "gl_memory.h"
#include "gl_pipeline.h"

#include <memory>

gl_texture_object* gl_find_texture_object(GLuint name)
{
	gl_named_object_node* node = gl_find_named_object(name);
	if (node == nullptr) return nullptr;
	gl_named_object* object = node->object;
	if (object == nullptr) return nullptr;
	if (object->type != __TEXTURE_OBJECT__)
	{
		return nullptr;
	}
	return (gl_texture_object*)object;
}

gl_texture2d* gl_create_texture2d()
{
	gl_texture2d* result = (gl_texture2d*)gl_malloc(sizeof(gl_texture2d));
	result->format = GL_TEXTURE_2D;
	result->mipmap_count = 0;
	result->mipmaps = (gl_texture2d_mipmap**)gl_malloc(sizeof(gl_texture2d_mipmap*)*(MAX_MIPMAP_LEVEL+1));
	memset(result->mipmaps, 0, sizeof(gl_texture2d_mipmap*)*(MAX_MIPMAP_LEVEL + 1));
	return result;
}

void gl_destory_texture2d(gl_texture2d* texture)
{
	for (GLint i = 0; i < MAX_MIPMAP_LEVEL; ++i)
	{
		if (texture->mipmaps[i] != nullptr)
		{
			gl_free(texture->mipmaps[i]);
		}
	}
}

gl_texture_cube* gl_create_texture_cube()
{
	gl_texture_cube* result = (gl_texture_cube*)gl_malloc(sizeof(gl_texture_cube));
	result->format = GL_TEXTURE_CUBE_MAP;
	result->mipmap_count = 0;
	result->mipmaps_x_positive = (gl_texture2d_mipmap**)gl_malloc(sizeof(gl_texture2d_mipmap*)*MAX_MIPMAP_LEVEL);
	result->mipmaps_x_nagetive = (gl_texture2d_mipmap**)gl_malloc(sizeof(gl_texture2d_mipmap*)*MAX_MIPMAP_LEVEL);
	result->mipmaps_y_positive = (gl_texture2d_mipmap**)gl_malloc(sizeof(gl_texture2d_mipmap*)*MAX_MIPMAP_LEVEL);
	result->mipmaps_y_nagetive = (gl_texture2d_mipmap**)gl_malloc(sizeof(gl_texture2d_mipmap*)*MAX_MIPMAP_LEVEL);
	result->mipmaps_z_positive = (gl_texture2d_mipmap**)gl_malloc(sizeof(gl_texture2d_mipmap*)*MAX_MIPMAP_LEVEL);
	result->mipmaps_z_nagetive = (gl_texture2d_mipmap**)gl_malloc(sizeof(gl_texture2d_mipmap*)*MAX_MIPMAP_LEVEL);
	memset(result->mipmaps_x_positive, 0, sizeof(gl_texture2d_mipmap*)*MAX_MIPMAP_LEVEL);
	memset(result->mipmaps_x_nagetive, 0, sizeof(gl_texture2d_mipmap*)*MAX_MIPMAP_LEVEL);
	memset(result->mipmaps_y_positive, 0, sizeof(gl_texture2d_mipmap*)*MAX_MIPMAP_LEVEL);
	memset(result->mipmaps_y_nagetive, 0, sizeof(gl_texture2d_mipmap*)*MAX_MIPMAP_LEVEL);
	memset(result->mipmaps_z_positive, 0, sizeof(gl_texture2d_mipmap*)*MAX_MIPMAP_LEVEL);
	memset(result->mipmaps_z_nagetive, 0, sizeof(gl_texture2d_mipmap*)*MAX_MIPMAP_LEVEL);
	return result;
}

GLfloat gl_get_wrap_value(GLfloat origin, GLenum wrap_mode,GLfloat min)
{
	switch (wrap_mode)
	{
	case GL_CLAMP_TO_EDGE:
		origin = glClamp(origin, min, 1.0f - min);
		break;
	case GL_REPEAT:
		origin -= std::floorf(origin);
		break;
	case GL_MIRRORED_REPEAT:
	{
		GLint floor_s = (GLint)std::floorf(origin);
		if (floor_s % 2 == 0)
		{
			origin = origin - floor_s;
		}
		else
		{
			origin = 1.0f - (origin - floor_s);
		}
	}
	break;
	}
	return origin;
}

void gl_sample_texture2d(GLuint index, GLfloat l, GLfloat s, GLfloat t, GLfloat* result)
{
	gl_texture_target& texture2d_target = glPipeline.texture_units[index].params.texture2d_target;
	gl_texture2d* texure2d = (gl_texture2d*)texture2d_target.binded_object->texture;
	
	GLfloat c = 0.0f;
	if (texture2d_target.mag_filter == GL_LINEAR && (texture2d_target.min_filter == GL_NEAREST_MIPMAP_LINEAR || texture2d_target.min_filter == GL_NEAREST_MIPMAP_NEAREST))
	{
		c = 0.5;
	}
	GLint level = 0;
	bool is_mag = l <= c;//mag
	if (!is_mag)//min
	{
		GLint q = texure2d->mipmap_count;
		if (l > 0.5f &&  l < (q + 0.5))
		{
			level = (GLint)std::ceil(l + 0.5) - 1;
		}
		else if (l > (q + 0.5f))
		{
			level = q;
		}
	}

	gl_texture2d_mipmap* mipmap = texure2d->mipmaps[level];
	GLsizei w = mipmap->width;
	GLsizei h = mipmap->height;
	GLfloat minx = 1.0f / (2.0f * w);
	GLfloat miny = 1.0f / (2.0f * h);
	s = gl_get_wrap_value(s, texture2d_target.wrap_mode_s, minx);
	t = gl_get_wrap_value(t, texture2d_target.wrap_mode_t, miny);

	GLfloat u = s * w;
	GLfloat v = t * h;

	if ((is_mag && texture2d_target.mag_filter == GL_NEAREST) || (!is_mag && texture2d_target.min_filter == GL_NEAREST))
	{
		GLsizei i = s == 1.0f ? (w - 1) : (GLsizei)std::floor(u);
		GLsizei j = t == 1.0f ? (h - 1) : (GLsizei)std::floor(v);
		*(gl_vector4*)result = *(gl_vector4*)mipmap->get_data(i, j);
	}
	else
	{
		GLfloat floor_u = std::floor(u - 0.5f);
		GLfloat floor_v = std::floor(v - 0.5f);
		GLsizei i0 = (GLsizei)floor_u;
		GLsizei j0 = (GLsizei)floor_v;
		GLsizei i1 = i0 + 1;
		GLsizei j1 = j0 + 1;
		gl_vector4 v00 = *(gl_vector4*)mipmap->get_data(i0, j0);
		gl_vector4 v01 = *(gl_vector4*)mipmap->get_data(i0, j1);
		gl_vector4 v10 = *(gl_vector4*)mipmap->get_data(i1, j0);
		gl_vector4 v11 = *(gl_vector4*)mipmap->get_data(i1, j1);
		GLfloat a = u - floor_u;
		GLfloat b = v - floor_v;
		*(gl_vector4*)result = (1.0f - a)*(1.0f - b)*v00 + a * (1.0f - b)*v10 + (1.0f - a)*b*v01 + a * b*v11;
	}
}

void gl_sample_texture_cube(GLuint index, GLfloat s, GLfloat t, GLfloat u, GLfloat* result)
{

}

GLvoid* gl_create_texture(GLenum type)
{
	switch (type)
	{
	case GL_TEXTURE_2D:			return gl_create_texture2d();
	case GL_TEXTURE_CUBE_MAP:	return gl_create_texture_cube();
	}
	return nullptr;
}

 void glPixelStorei(GLenum pname, GLint param)
{
	switch (pname)
	{
	case GL_UNPACK_ALIGNMENT:
	{
		switch (param)
		{
		case UNPACK_ALIGNMENT_1:
		case UNPACK_ALIGNMENT_2:
		case UNPACK_ALIGNMENT_4:
		case UNPACK_ALIGNMENT_8:
			glContext.unpack_mode = param;
			break;
		default:
			glSetError(GL_INVALID_VALUE, "Invalid unpack alignment value!");
			break;
		}
	}
		break;
	default:
		glSetError(GL_INVALID_ENUM, "Invalid enum value for glPixelStorei'pname!");
		break;
	}
}
 void glActiveTexture(GLenum texture)
{
	GLenum index = texture - GL_TEXTURE0;
	if (index > (MAX_COMBINED_TEXTURE_IMAGE_UNITS - 1))
	{
		glSetError(GL_INVALID_VALUE, "Invalid texture param!");
		return;
	}
	glContext.selected_texture_unit = &glContext.texture_params[index];
}

void gl_read_byte_type(GLfloat* data, GLsizei width, GLsizei height, GLenum format, const GLubyte *pixels)
{
	//todo alignment
	switch (format)
	{
	case GL_ALPHA:
	{
		for (GLsizei y = 0; y < height; ++y)
		{
			for (GLsizei x = 0; x < width; ++x)
			{
				GLsizei index_src = y * width + x;
				GLsizei index_det = ((height - y - 1) * width + x) * 4;
				GLubyte alpha = pixels[index_src];
				GLfloat nomalized_alpha = alpha / 255.0f;
				data[index_det + 0] = 0.0f;
				data[index_det + 1] = 0.0f;
				data[index_det + 2] = 0.0f;
				data[index_det + 3] = nomalized_alpha;
			}
		}
		break;
	}
	case GL_LUMINANCE:
	{
		for (GLsizei y = 0; y < height; ++y)
		{
			for (GLsizei x = 0; x < width; ++x)
			{
				GLsizei index_src = y * width + x;
				GLsizei index_det = ((height - y - 1) * width + x) * 4;
				GLubyte luminance = pixels[index_src];
				GLfloat nomalized_luminance = luminance / 255.0f;
				data[index_det + 0] = nomalized_luminance;
				data[index_det + 1] = nomalized_luminance;
				data[index_det + 2] = nomalized_luminance;
				data[index_det + 3] = 1.0f;
			}
		}
		break;
	}
	case GL_LUMINANCE_ALPHA:
	{
		for (GLsizei y = 0; y < height; ++y)
		{
			for (GLsizei x = 0; x < width; ++x)
			{
				GLsizei index_src = (y * width + x) * 2;
				GLsizei index_det = ((height - y - 1) * width + x) * 4;
				GLubyte luminance =	pixels[index_src + 0];
				GLubyte alpha =		pixels[index_src + 1];
				GLfloat nomalized_luminance		= luminance / 255.0f;
				GLfloat nomalized_alpha			= alpha		/ 255.0f;
				data[index_det + 0] = nomalized_luminance;
				data[index_det + 1] = nomalized_luminance;
				data[index_det + 2] = nomalized_luminance;
				data[index_det + 3] = alpha;
			}
		}
		break;
	}
	case GL_RGB:
	{
		for (GLsizei y = 0; y < height; ++y)
		{
			for (GLsizei x = 0; x < width; ++x)
			{
				GLsizei index_src = (y * width + x) * 3;
				GLsizei index_det = ((height - y - 1) * width + x) * 4;
				GLubyte r = pixels[index_src + 0];
				GLubyte g = pixels[index_src + 1];
				GLubyte b = pixels[index_src + 2];
				GLfloat nomalized_r = r / 255.0f;
				GLfloat nomalized_g = g / 255.0f;
				GLfloat nomalized_b = b / 255.0f;
				data[index_det + 0] = nomalized_r;
				data[index_det + 1] = nomalized_g;
				data[index_det + 2] = nomalized_b;
				data[index_det + 3] = 1.0f;
			}
		}
		break;
	}
	case GL_RGBA:
	{
		for (GLsizei y = 0; y < height; ++y)
		{
			for (GLsizei x = 0; x < width; ++x)
			{
				GLsizei index_src = (y * width + x) * 4;
				GLsizei index_det = ((height - y - 1) * width + x) * 4;
				GLubyte r = pixels[index_src + 0];
				GLubyte g = pixels[index_src + 1];
				GLubyte b = pixels[index_src + 2];
				GLubyte a = pixels[index_src + 3];
				GLfloat nomalized_r = r / 255.0f;
				GLfloat nomalized_g = g / 255.0f;
				GLfloat nomalized_b = b / 255.0f;
				GLfloat nomalized_a = b / 255.0f;
				data[index_det + 0] = nomalized_r;
				data[index_det + 1] = nomalized_g;
				data[index_det + 2] = nomalized_b;
				data[index_det + 3] = nomalized_a;
			}
		}
		break;
	}
	}
}

void gl_read_short_type(GLfloat* data, GLsizei width, GLsizei height, GLenum type, const GLushort *pixels)
{
	switch (type)
	{
	case GL_UNSIGNED_SHORT_5_6_5:
	{
		for (GLsizei y = 0; y < height; ++y)
		{
			for (GLsizei x = 0; x < width; ++x)
			{
				GLsizei index = y * width + x;
				GLsizei index_det = ((height - y - 1) * width + x) * 4;
				GLshort pixel = pixels[index];
				GLubyte r = pixel & 0b1111100000000000;
				GLubyte g = pixel & 0b0000011111100000;
				GLubyte b = pixel & 0b0000000000011111;
				GLfloat nomalized_r = r / 255.0f;
				GLfloat nomalized_g = g / 255.0f;
				GLfloat nomalized_b = b / 255.0f;
				data[index_det + 0] = nomalized_r;
				data[index_det + 1] = nomalized_g;
				data[index_det + 2] = nomalized_b;
				data[index_det + 3] = 1.0f;
			}
		}
	}
		break;
	case GL_UNSIGNED_SHORT_4_4_4_4:
	{
		for (GLsizei y = 0; y < height; ++y)
		{
			for (GLsizei x = 0; x < width; ++x)
			{
				GLsizei index = y * width + x;
				GLsizei index_det = ((height - y - 1) * width + x) * 4;
				GLshort pixel = pixels[index];
				GLubyte r = pixel & 0b1111000000000000;
				GLubyte g = pixel & 0b0000111100000000;
				GLubyte b = pixel & 0b0000000011110000;
				GLubyte a = pixel & 0b0000000000001111;
				GLfloat nomalized_r = r / 255.0f;
				GLfloat nomalized_g = g / 255.0f;
				GLfloat nomalized_b = b / 255.0f;
				GLfloat nomalized_a = a / 255.0f;
				data[index_det + 0] = nomalized_r;
				data[index_det + 1] = nomalized_g;
				data[index_det + 2] = nomalized_b;
				data[index_det + 3] = nomalized_a;
			}
		}
	}
		break;
	case GL_UNSIGNED_SHORT_5_5_5_1:
	{
		for (GLsizei y = 0; y < height; ++y)
		{
			for (GLsizei x = 0; x < width; ++x)
			{
				GLsizei index_src = y * width + x;
				GLsizei index_det = ((height - y - 1) * width + x)*4;
				GLshort pixel = pixels[index_src];
				GLubyte r = pixel & 0b1111100000000000;
				GLubyte g = pixel & 0b0000011111000000;
				GLubyte b = pixel & 0b0000000000111110;
				GLubyte a = pixel & 0b0000000000000001;
				GLfloat nomalized_r = r / 255.0f;
				GLfloat nomalized_g = g / 255.0f;
				GLfloat nomalized_b = b / 255.0f;
				GLfloat nomalized_a = a / 255.0f;
				data[index_det + 0] = nomalized_r;
				data[index_det + 1] = nomalized_g;
				data[index_det + 2] = nomalized_b;
				data[index_det + 3] = nomalized_a;
			}
		}
	}
		break;
	}
}

void gl_allocate_texture2d_mipmap(gl_texture2d_mipmap** pmipmap, GLint internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels)
{
	gl_texture2d_mipmap* mipmap = *pmipmap;
	if (mipmap != nullptr)
	{
		gl_free(mipmap->data);
	}
	else
	{
		mipmap = (gl_texture2d_mipmap*)gl_malloc(sizeof(gl_texture2d_mipmap));
		*pmipmap = mipmap;
	}
	mipmap->width = width;
	mipmap->height = height;
	mipmap->data = (GLfloat*)gl_malloc((width * height * 4) * sizeof(GLfloat));
	switch (type)
	{
	case GL_UNSIGNED_BYTE:
		gl_read_byte_type((GLfloat*)mipmap->data, width, height, format, (const GLubyte*)pixels);
		break;
	case GL_UNSIGNED_SHORT_5_6_5:
	case GL_UNSIGNED_SHORT_4_4_4_4:
	case GL_UNSIGNED_SHORT_5_5_5_1:
		gl_read_short_type((GLfloat*)mipmap->data, width, height, type, (const GLushort*)pixels);
		break;
	}
}

 void glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels)
{
	CONDITION_VALIDATE(level < 0 || level > MAX_MIPMAP_LEVEL, GL_INVALID_VALUE, "Invalid level params!");
	GLsizei max_width_for_level = 1 << (MAX_MIPMAP_LEVEL - level);
	CONDITION_VALIDATE(width < 0 || height < 0, GL_INVALID_VALUE, "Invalid width height param,must not less than 0!");
	CONDITION_VALIDATE(width > max_width_for_level || height > max_width_for_level, GL_INVALID_VALUE, "Invalid width height param,greater than maximum!");
	CONDITION_VALIDATE(border != 0, GL_INVALID_VALUE, "Invalid border param, must be zero!");

	switch (type)
	{
	case GL_UNSIGNED_BYTE:
		break;
	case GL_UNSIGNED_SHORT_5_6_5:
		CONDITION_VALIDATE(format != GL_RGB, GL_INVALID_VALUE, "format and type dosen't match!");
		break;
	case GL_UNSIGNED_SHORT_4_4_4_4:
		CONDITION_VALIDATE(format != GL_RGBA, GL_INVALID_VALUE, "format and type dosen't match!");
		break;
	case GL_UNSIGNED_SHORT_5_5_5_1:
		CONDITION_VALIDATE(format != GL_RGBA, GL_INVALID_VALUE, "format and type dosen't match!");
		break;
	default:
	{
		glSetError(GL_INVALID_VALUE, "Invalid type param!");
		return;
		break;
	}
	}

	//GLint mipmap_count = 

	switch (target)
	{
	case GL_TEXTURE_2D:
	{
		gl_texture2d* texture = (gl_texture2d*)glContext.selected_texture_unit->texture2d_target.binded_object->texture;
		CONDITION_VALIDATE(texture == nullptr, GL_INVALID_OPERATION, "The selected texture is not binded!");
		gl_allocate_texture2d_mipmap(&texture->mipmaps[level], internalformat, width, height, format, type, pixels);
		break;
	}
	case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
	{
		gl_texture_cube* texture = (gl_texture_cube*)glContext.selected_texture_unit->texture_cube_target.binded_object->texture;
		CONDITION_VALIDATE(texture == nullptr, GL_INVALID_OPERATION, "The selected texture is not binded!");
		gl_allocate_texture2d_mipmap(&texture->mipmaps_x_positive[level], internalformat, width, height, format, type, pixels);
	}
	break;
	case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
	{
		gl_texture_cube* texture = (gl_texture_cube*)glContext.selected_texture_unit->texture_cube_target.binded_object->texture;
		CONDITION_VALIDATE(texture == nullptr, GL_INVALID_OPERATION, "The selected texture is not binded!");
		gl_allocate_texture2d_mipmap(&texture->mipmaps_x_nagetive[level], internalformat, width, height, format, type, pixels);
	}
	break;
	case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
	{
		gl_texture_cube* texture = (gl_texture_cube*)glContext.selected_texture_unit->texture_cube_target.binded_object->texture;
		CONDITION_VALIDATE(texture == nullptr, GL_INVALID_OPERATION, "The selected texture is not binded!");
		gl_allocate_texture2d_mipmap(&texture->mipmaps_y_positive[level], internalformat, width, height, format, type, pixels);
	}
	break;
	case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
	{
		gl_texture_cube* texture = (gl_texture_cube*)glContext.selected_texture_unit->texture_cube_target.binded_object->texture;
		CONDITION_VALIDATE(texture == nullptr, GL_INVALID_OPERATION, "The selected texture is not binded!");
		gl_allocate_texture2d_mipmap(&texture->mipmaps_y_nagetive[level], internalformat, width, height, format, type, pixels);
	}
	break;
	case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
	{
		gl_texture_cube* texture = (gl_texture_cube*)glContext.selected_texture_unit->texture_cube_target.binded_object->texture;
		CONDITION_VALIDATE(texture == nullptr, GL_INVALID_OPERATION, "The selected texture is not binded!");
		gl_allocate_texture2d_mipmap(&texture->mipmaps_z_positive[level], internalformat, width, height, format, type, pixels);
	}
	break;
	case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
	{
		gl_texture_cube* texture = (gl_texture_cube*)glContext.selected_texture_unit->texture_cube_target.binded_object->texture;
		CONDITION_VALIDATE(texture == nullptr, GL_INVALID_OPERATION, "The selected texture is not binded!");
		gl_allocate_texture2d_mipmap(&texture->mipmaps_z_nagetive[level], internalformat, width, height, format, type, pixels);
	}
	break;
	default:
	{
		glSetError(GL_INVALID_ENUM, "Invalid target param!");
		break;
	}
	}

}

bool is_power_of_2(GLint value)
{
	return (value & (value - 1)) == 0;
}

GLint log2(GLint value)
{
	GLint n = 0;
	while ((value >>= 1) != 0)
	{
		++n;
	}
	return n;
}

void gl_generate_mipmap(const gl_texture2d_mipmap* pre_mipmap, gl_texture2d_mipmap** pcur_mipmap, GLint level)
{
	gl_texture2d_mipmap* cur_mipmap = *pcur_mipmap;
	if (cur_mipmap == nullptr)
	{
		cur_mipmap = (gl_texture2d_mipmap*)gl_malloc(sizeof(gl_texture2d_mipmap));
		cur_mipmap->data = nullptr;
		*pcur_mipmap = cur_mipmap;
	}
	if (cur_mipmap->data != nullptr)
	{
		gl_free(cur_mipmap->data);
	}

	GLint pre_w = pre_mipmap->width;
	GLint pre_h = pre_mipmap->height;

	GLint cur_w = pre_w >> 1;
	GLint cur_h = pre_h >> 1;

	cur_mipmap->width = cur_w;
	cur_mipmap->height = cur_h;

	if (cur_w <= 0) cur_w = 1;
	if (cur_h <= 0) cur_h = 1;

	cur_mipmap->data = (GLfloat*)gl_malloc((cur_w*cur_h * 4) * sizeof(GLfloat));

	const GLfloat* pre_data = pre_mipmap->data;
	GLfloat* cur_data = cur_mipmap->data;
	for (GLint y = 0; y < cur_h; ++y)
	{
		for (GLint x = 0; x < cur_w; ++x)
		{
			GLint pre_x = x << 1;
			GLint pre_y = y << 1;

			GLint offset1 = ((pre_y + 0)*pre_w + pre_x + 0) * 4;
			GLint offset2 = ((pre_y + 0)*pre_w + pre_x + 1) * 4;
			GLint offset3 = ((pre_y + 1)*pre_w + pre_x + 0) * 4;
			GLint offset4 = ((pre_y + 1)*pre_w + pre_x + 1) * 4;

			GLfloat pre_r1 = pre_data[offset1 + 0];
			GLfloat pre_g1 = pre_data[offset1 + 1];
			GLfloat pre_b1 = pre_data[offset1 + 2];
			GLfloat pre_a1 = pre_data[offset1 + 3];

			GLfloat pre_r2 = pre_data[offset2 + 0];
			GLfloat pre_g2 = pre_data[offset2 + 1];
			GLfloat pre_b2 = pre_data[offset2 + 2];
			GLfloat pre_a2 = pre_data[offset2 + 3];

			GLfloat pre_r3 = pre_data[offset3 + 0];
			GLfloat pre_g3 = pre_data[offset3 + 1];
			GLfloat pre_b3 = pre_data[offset3 + 2];
			GLfloat pre_a3 = pre_data[offset3 + 3];

			GLfloat pre_r4 = pre_data[offset4 + 0];
			GLfloat pre_g4 = pre_data[offset4 + 1];
			GLfloat pre_b4 = pre_data[offset4 + 2];
			GLfloat pre_a4 = pre_data[offset4 + 3];

			cur_data[(y*cur_w + x) * 4 + 0] = (pre_r1 + pre_r2 + pre_r3 + pre_r4) / 4.0f;
			cur_data[(y*cur_w + x) * 4 + 1] = (pre_g1 + pre_g2 + pre_g3 + pre_g4) / 4.0f;
			cur_data[(y*cur_w + x) * 4 + 2] = (pre_b1 + pre_b2 + pre_b3 + pre_b4) / 4.0f;
			cur_data[(y*cur_w + x) * 4 + 3] = (pre_a1 + pre_a2 + pre_a3 + pre_a4) / 4.0f;

		}
	}
}

void gl_generate_texture2d_mipmap(gl_texture2d* texture)
{
	GLint w0 = texture->mipmaps[0]->width;
	GLint h0 = texture->mipmaps[0]->height;

	GLint max_size = glMax(w0, h0);
	GLint level = log2(max_size);
	texture->mipmap_count = level;
	for (GLint l = 1; l <= level; ++l)
	{
		gl_generate_mipmap(texture->mipmaps[l-1], &texture->mipmaps[l], l);
	}
}

 void glGenerateMipmap(GLenum target)
{
	switch (target)
	{
	case GL_TEXTURE_2D:
	{
		gl_texture2d* texture = (gl_texture2d*)glContext.selected_texture_unit->texture2d_target.binded_object->texture;
		CONDITION_VALIDATE(texture == nullptr, GL_INVALID_OPERATION, "The selected texture is not binded!");
		CONDITION_VALIDATE(texture->mipmaps[0] == nullptr, GL_INVALID_VALUE, "The selected texture has no level 0 data!");
		CONDITION_VALIDATE(!is_power_of_2(texture->mipmaps[0]->width), GL_INVALID_VALUE, "The width is not power of 2!");
		CONDITION_VALIDATE(!is_power_of_2(texture->mipmaps[0]->height), GL_INVALID_VALUE, "The height is not power of 2!");
		gl_generate_texture2d_mipmap(texture);
		break;
	}
	case GL_TEXTURE_CUBE_MAP:
	{
		gl_texture_cube* texture = (gl_texture_cube*)glContext.selected_texture_unit->texture_cube_target.binded_object->texture;
		CONDITION_VALIDATE(texture == nullptr, GL_INVALID_OPERATION, "The selected texture is not binded!");
		CONDITION_VALIDATE(texture->mipmaps_x_positive[0] == nullptr, GL_INVALID_OPERATION, "The selected texture has no level 0 data!");
		CONDITION_VALIDATE(texture->mipmaps_x_nagetive[0] == nullptr, GL_INVALID_OPERATION, "The selected texture has no level 0 data!");
		CONDITION_VALIDATE(texture->mipmaps_y_positive[0] == nullptr, GL_INVALID_OPERATION, "The selected texture has no level 0 data!");
		CONDITION_VALIDATE(texture->mipmaps_y_nagetive[0] == nullptr, GL_INVALID_OPERATION, "The selected texture has no level 0 data!");
		CONDITION_VALIDATE(texture->mipmaps_z_positive[0] == nullptr, GL_INVALID_OPERATION, "The selected texture has no level 0 data!");
		CONDITION_VALIDATE(texture->mipmaps_z_nagetive[0] == nullptr, GL_INVALID_OPERATION, "The selected texture has no level 0 data!");
		break;
	}
	default:
	{
		glSetError(GL_INVALID_ENUM, "Invalid target param!");
		break;
	}
	}
}

 void glBindTexture(GLenum target, GLuint texture)
{
	CONDITION_VALIDATE(target != GL_TEXTURE_2D && target != GL_TEXTURE_CUBE_MAP, GL_INVALID_ENUM, "Invalid target!");
	gl_texture_object* object = gl_find_texture_object(texture);
	CONDITION_VALIDATE(object == nullptr, GL_INVALID_VALUE, "Invalid texture parameter!");
	CONDITION_VALIDATE(object->type != 0 && object->type != target, GL_INVALID_OPERATION, "Type not match!");
	//todo target validity check
	if (object->type == 0)
	{
		object->type = target;
		object->texture = gl_create_texture(object->type);
	}
	
	switch (target)
	{
	case GL_TEXTURE_2D:
	{
		glContext.selected_texture_unit->texture2d_target.binded_object = object;
		break;
	}
	case GL_TEXTURE_CUBE_MAP:
	{
		glContext.selected_texture_unit->texture_cube_target.binded_object = object;
		break;
	}
	}
}

void glDeleteTextures(GLsizei n, const GLuint *textures)
{
	CONDITION_VALIDATE(textures == nullptr, GL_INVALID_VALUE, "Invalid textures parameter!");
	for (GLsizei i = 0; i < n; ++i)
	{
		gl_texture_object* object = gl_find_texture_object(textures[i]);
		if (object != nullptr)
		{
			//check binding
			//release_texture
			gl_destroy_named_object(textures[i]);
		}
	}
}

void glGenTextures(GLsizei n, GLuint *textures)
{
	CONDITION_VALIDATE(textures == nullptr, GL_INVALID_VALUE, "Invalid textures parameter!");
	for (GLsizei i = 0; i < n; ++i)
	{
		textures[i] = gl_create_named_object(__TEXTURE_OBJECT__);
	}
}

void gl_set_texture_parameter(gl_texture_target& target, GLenum pname, GLint param)
{
	switch (pname)
	{
	case GL_TEXTURE_WRAP_S:
	{
		target.wrap_mode_s = param;
		break;
	}
	case GL_TEXTURE_WRAP_T:
	{
		target.wrap_mode_t = param;
		break;
	}
	case GL_TEXTURE_MIN_FILTER:
	{
		target.min_filter = param;
		break;
	}
	case GL_TEXTURE_MAG_FILTER:
	{
		target.mag_filter = param;
		break;
	}
	default:
		CONDITION_VALIDATE(true, GL_INVALID_ENUM, "Invalid pname");
		break;
	}
}

void glTexParameteri(GLenum target, GLenum pname, GLint param)
{
	switch (target)
	{
	case GL_TEXTURE_2D:
	{
		gl_texture_target& target = glContext.selected_texture_unit->texture2d_target;
		gl_set_texture_parameter(target, pname, param);
		break;
	}
	case GL_TEXTURE_CUBE_MAP:
	{
		break;
	}
	default:
		CONDITION_VALIDATE(true, GL_INVALID_ENUM, "Invalid target");
		break;
	}
}

void glTexParameterf(GLenum target, GLenum pname, GLfloat param)
{

}
