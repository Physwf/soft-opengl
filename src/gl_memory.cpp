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

#include "gl_memory.h"
#include <stdlib.h>

#include "gl_pipeline.h"

int gl_minit()
{
	return 0;
}

void* gl_malloc(GLuint iSize,const char* name/* = nullptr*/)
{
	void* result = ::malloc(iSize);
	//glPpeline.data[result] = iSize;
// 	if (name)
// 	{
// 		glPpeline.datanames[result] = name;
// 	}
	return result;
}

void* gl_realloc(void* pOrignal, GLuint size)
{
	return ::realloc(pOrignal, size);
}

void gl_free(void* pData)
{
	//glPpeline.datanames.erase(pData);
	//glPpeline.data.erase(pData);
	::free(pData);
}
