#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

bool Texture2D::LoadFromJpg(const char* fileName)
{
	unsigned char *data = stbi_load(fileName, &width, &height, &nrChannels, 0);
	if (data)
	{
		int datasize = width * height* nrChannels;
		RawData.resize(datasize);
		memcpy_s(RawData.data(), datasize, data, datasize);
		stbi_image_free(data);
		return true;
	}
	return false;
}

void Texture2D::UpdateRHI()
{
	if (name != 0)
	{
		glDeleteTextures(1, &name);
	}
	glGenTextures(1, &name);
	glBindTexture(GL_TEXTURE_2D, name);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, RawData.data());
	glGenerateMipmap(GL_TEXTURE_2D);
}

