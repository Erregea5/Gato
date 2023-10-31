#include "VertexBuffer.h"
#include "Renderer.h"

namespace Graphics {
	VertexBuffer::VertexBuffer(const void* data, unsigned int size)
	{
		GLCall(glGenBuffers(1, &ID));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, ID));
		GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
	}
	VertexBuffer::VertexBuffer()
	{
		GLCall(glGenBuffers(1, &ID));
	}
	void VertexBuffer::init(const void* data, unsigned int size)
	{
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, ID));
		GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
	}

	VertexBuffer::~VertexBuffer()
	{
		GLCall(glDeleteBuffers(1, &ID));
	}

	void VertexBuffer::bind() const
	{
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, ID));
	}

	void VertexBuffer::unBind() const
	{
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}
}
