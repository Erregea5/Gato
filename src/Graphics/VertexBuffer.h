#pragma once

namespace Graphics {
	class VertexBuffer
	{
	private:
		unsigned int ID;
	public:
		VertexBuffer(const void* data, unsigned int size);
		VertexBuffer();
		void init(const void* data, unsigned int size);
		~VertexBuffer();

		void bind() const;
		void unBind() const;
	};
}