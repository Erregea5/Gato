#pragma once
#include <stdexcept>
#include <iostream>
#include <vector>
#include <GL/glew.h>

namespace Graphics {
	struct VertexBufferElement
	{
		unsigned int type;
		unsigned int count;
		unsigned char normalized;

		unsigned int getSizeOfType() const
		{
			switch (type) {
			case GL_FLOAT:
				return sizeof(float);
			case GL_UNSIGNED_INT:
				return sizeof(unsigned int);
			}
			std::cout << "error in VertexBufferElement getSizeOfType" << std::endl;
			return 0;
		}
		static unsigned int getSizeOfType(unsigned int type)
		{
			switch (type) {
			case GL_FLOAT:
				return sizeof(float);
			case GL_UNSIGNED_INT:
				return sizeof(unsigned int);
			}
			std::cout << "error in VertexBufferElement getSizeOfType" << std::endl;
			return 0;
		}
	};

	class VertexBufferLayout
	{
	private:
		std::vector<VertexBufferElement> elements;
		unsigned int stride;
	public:
		VertexBufferLayout() :stride(0) {}

		void push(unsigned int type, unsigned int count)
		{
			elements.push_back({ type,count,GL_FALSE });
			stride += VertexBufferElement::getSizeOfType(type) * count;
		}
		const std::vector<VertexBufferElement>& getElements() const
		{
			return elements;
		}
		unsigned int getStride() const
		{
			return stride;
		}
	};
}
