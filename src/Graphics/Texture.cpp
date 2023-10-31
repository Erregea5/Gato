#include "Texture.h"
#include "Renderer.h"
#include <iostream>
#include <unordered_map>

namespace Graphics {
	Texture::Texture(const std::string& _filePath)
		:filePaths(1, _filePath), isCube(false)
	{construct();}

	Texture::Texture(const std::vector<std::string>& _filePaths)
		:isCube(true),filePaths(_filePaths)
	{constructCube();}

	Texture::Texture(const Texture& rhs)
		:filePaths(rhs.filePaths),isCube(rhs.isCube)
	{
		if (isCube)
			constructCube();
		else
			construct();
	}

	Texture& Texture::operator=(const Texture& rhs)
	{
		filePaths = (rhs.filePaths);
		isCube = (rhs.isCube);
		if (isCube)
			constructCube();
		else
			construct();
		return *this;
	}

	Texture::~Texture()
	{
		if (ID != 0)
			GLCall(glDeleteTextures(1, &ID));
	}

	void Texture::bind(unsigned int slot) const
	{
		if (ID != 0)
			GLCall(glActiveTexture(GL_TEXTURE0 + slot));
		if (isCube) {
			GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, ID));
		}
		else
			GLCall(glBindTexture(GL_TEXTURE_2D, ID));
	}

	void Texture::unBind() const
	{
		if (isCube) {
			GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
		}
		else
			GLCall(glBindTexture(GL_TEXTURE_2D, 0));
	}

	void Texture::unBind(int)
	{
		GLCall(glBindTexture(GL_TEXTURE_2D, 0));
		GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
	}

	void Texture::construct()
	{
		if (filePaths[0] == "")
		{
			ID = width = height = BPP = 0;
			return;
		}
		GLCall(glGenTextures(1, &ID));
		GLCall(glBindTexture(GL_TEXTURE_2D, ID));

		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));

		unsigned char* data = 0;
		stbi_set_flip_vertically_on_load(1);
		data = stbi_load(filePaths[0].c_str(), &width, &height, &BPP, 4);
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));
		unBind();

		if (data)
			stbi_image_free(data);
	}

	int fitToPowerOfTwo(int width, int height)
	{
		double min = width < height ? height : width;
		return (int)pow(2.0, ceil(log2(min)));
	}

	void Texture::constructCube()
	{
		std::unordered_map<std::string, int> imageCache;
		GLCall(glGenTextures(1, &ID));
		GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, ID));

		GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT));
		GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT));
		GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT));

		unsigned char* data[6] = { 0,0,0,0,0,0 };
		stbi_set_flip_vertically_on_load(0);
		for (unsigned int i = 0; i < 6; i++)
		{
			int index = i;
			unsigned int internalFormat = GL_RGBA8,
				format = GL_RGBA;

			if (imageCache.find(filePaths[i]) != imageCache.end())
				index = imageCache[filePaths[i]];
			else
			{
				data[i] = stbi_load(filePaths[i].c_str(), &width, &height, &BPP, 0);
				imageCache[filePaths[i]] = i;
			}
			if (BPP == 3) {
				internalFormat = GL_RGB8;
				format = GL_RGB;
			}
			if (data[index])
			{
				int po2 = fitToPowerOfTwo(width, height);
				if (width == height && height == po2)
				{
					GLCall(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, po2, po2, 0, format, GL_UNSIGNED_BYTE, data[index]));
				}
				else
				{
					GLCall(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, po2, po2, 0, format, GL_UNSIGNED_BYTE, NULL));
					GLCall(glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, data[index]));
				}
			}
			else
				std::cout << "Cubemap texture failed to load at path: " << filePaths[i] << std::endl;
		}
		unBind();
		for (auto& i : imageCache)
			stbi_image_free(data[i.second]);
	}
}