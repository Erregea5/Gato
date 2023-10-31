#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "Renderer.h"

namespace Graphics {
	VertexArray::VertexArray()
	{
#ifndef __EMSCRIPTEN__
		GLCall(glGenVertexArrays(1, &ID));
#endif
	}

	VertexArray::~VertexArray()
	{
#ifndef __EMSCRIPTEN__
		GLCall(glDeleteVertexArrays(1, &ID));
#endif
	}

	void VertexArray::bind() const
	{
#ifdef __EMSCRIPTEN__
		addBufferInternal(buffer, layout);
#else
		GLCall(glBindVertexArray(ID));
#endif
	}

	void VertexArray::unBind() const
	{
#ifdef __EMSCRIPTEN__
		buffer.unBind();
#else
		GLCall(glBindVertexArray(0));
#endif
	}

	void VertexArray::addBuffer(const VertexBuffer& _buffer, const VertexBufferLayout& _layout)
	{
#ifdef __EMSCRIPTEN__
		buffer = _buffer;
		layout = _layout;
#else
		bind();
		addBufferInternal(_buffer, _layout);
#endif
	}

	void VertexArray::addBufferInternal(const VertexBuffer& _buffer, const VertexBufferLayout& _layout) const
	{
		_buffer.bind();
		const auto& elements = _layout.getElements();
		unsigned int offset = 0;
		for (unsigned int i = 0;i < elements.size();i++) {
			VertexBufferElement e = elements[i];
			GLCall(glEnableVertexAttribArray(i));
			GLCall(glVertexAttribPointer(i, e.count, e.type, e.normalized, _layout.getStride(), (const void*)offset));
			offset += e.count * e.getSizeOfType();
		}
	}
}
