#pragma once
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

namespace Graphics {
	class VertexArray
	{
	private:
#ifdef __EMSCRIPTEN__
		//simulated VertexArray
		VertexBufferLayout layout;
		VertexBuffer buffer;
#else
		unsigned int ID;
#endif
	public:
		VertexArray();
		~VertexArray();

		void bind() const;
		void unBind() const;

		void addBuffer(const VertexBuffer& _buffer, const VertexBufferLayout& _layout);
	private:
		void addBufferInternal(const VertexBuffer& _buffer, const VertexBufferLayout& _layout) const;
	};
}