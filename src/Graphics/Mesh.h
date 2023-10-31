#pragma once
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Texture.h"
#include <string>
#include <vector>

namespace Graphics {
	class Mesh
	{
	public:
		enum class PreMade{
/*3D*/		Cube, Cone, Capsule, Cylinder, Sphere, Icosahedron, Quartz,
/*2D*/		Disk, Square
		};
	public:
		std::vector<float> vertices;
		std::vector<unsigned int> indices;
		int dim;
		bool hasNormals,
			hasTexture;


		VertexArray vao;
		IndexBuffer ibo;
		VertexBuffer vb;
		VertexBufferLayout layout;

		std::string filePath;
	public:
		Mesh(const std::string& file, int dim = 3, bool _hasNormals = true, bool _hasTexture = true);
		Mesh(const std::vector<float>& _vertices, const std::vector<unsigned int>& _indices, int dim = 3, bool _hasNormals = true, bool _hasTexture = true);
		Mesh(PreMade mesh = PreMade::Cube);
		Mesh(const Mesh& rhs);
		Mesh& operator=(const Mesh& rhs);
		~Mesh();

		void update();

		void bind() const;
		void unBind() const;

		int getIboCount() const { return ibo.getCount(); }
	private:
		void loadObj(bool hasNormals, bool hasTexture);
	};
}