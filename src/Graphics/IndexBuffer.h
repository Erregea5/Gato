#pragma once

namespace Graphics {
	class IndexBuffer
	{
	private:
		unsigned int ID,
			count;
	public:
		IndexBuffer(const unsigned int* data, unsigned int count);
		IndexBuffer();
		void init(const unsigned int* data, unsigned int count);
		~IndexBuffer();

		void bind() const;
		void unBind() const;

		unsigned int getCount() const;
	};
}