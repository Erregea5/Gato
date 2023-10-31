#include "IndexBuffer.h"
#include "Renderer.h"

namespace Graphics {
	IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int _count)
	{
		count = _count;
		GLCall(glGenBuffers(1, &ID));
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID));
		GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, _count * sizeof(unsigned int), data, GL_STATIC_DRAW));
	}

	IndexBuffer::IndexBuffer()
	{
		count = 0;
		GLCall(glGenBuffers(1, &ID));
	}

	void IndexBuffer::init(const unsigned int* data, unsigned int _count)
	{
		count = _count;
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID));
		GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, _count * sizeof(unsigned int), data, GL_STATIC_DRAW));
	}

	IndexBuffer::~IndexBuffer()
	{
		GLCall(glDeleteBuffers(1, &ID));
	}

	void IndexBuffer::bind() const
	{
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, ID));
	}

	void IndexBuffer::unBind() const
	{
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	}

	unsigned int IndexBuffer::getCount() const
	{
		return count;
	}
}