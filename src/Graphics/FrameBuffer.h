#pragma once
#include "Texture.h"

namespace Graphics {
	class FrameBuffer
	{
	private:
		unsigned int ID;

	public:
		int width,
			height;
		bool hasColor, hasDepth;

		FrameBuffer(Texture& color, Texture& depth, int _width, int _height, bool cubeTexture = false);
		FrameBuffer(Texture& color, bool , int _width, int _height, bool cubeTexture = false);
		FrameBuffer(bool, Texture& depth, int _width, int _height, bool cubeTexture = false);
		~FrameBuffer();

		void bind() const;
		void unBind() const;
	private:
		void init2D(Texture* color, Texture* depth);
#ifndef __EMSCRIPTEN__
		void initCube(Texture* color, Texture* depth);
#endif
	};
}