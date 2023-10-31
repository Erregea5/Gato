#pragma once
#include <string>
#include <vector>

namespace Graphics {

	class Texture
	{
	public:
		unsigned int ID;
		std::vector<std::string> filePaths;
		int	width,
			height,
			BPP;
		bool isCube;
	public:
		Texture(const std::string& _filePath = "");
		Texture(const std::vector<std::string>& _filePaths);
		Texture(const Texture& rhs);
		Texture& operator=(const Texture& rhs);
		~Texture();

		void bind(unsigned int slot = 0) const;
		void unBind() const;
		static void unBind(int);

		int getHeight() const { return height; }
		int getWidth() const { return width; }
	private:
		void construct();
		void constructCube();
	};
}