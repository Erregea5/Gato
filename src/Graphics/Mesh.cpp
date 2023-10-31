#include "Mesh.h"
#include "VertexBufferLayout.h"
#include <glm/glm.hpp>
#include <unordered_map>
#include <fstream>
#include <sstream>


namespace Graphics {
	Mesh::Mesh(const std::string& file, int _dim, bool _hasNormals, bool _hasTexture)
		:filePath(file), dim(_dim), hasNormals(_hasNormals), hasTexture(_hasTexture)
	{
		loadObj(_hasNormals, _hasTexture);
		update();
	}

	Mesh::Mesh(const std::vector<float>& _vertices, const std::vector<unsigned int>& _indices, int _dim, bool _hasNormals, bool _hasTexture)
		:vertices(_vertices), indices(_indices), dim(_dim), hasNormals(_hasNormals), hasTexture(_hasTexture), filePath("")
	{update();}

	namespace {
		std::vector<float> squareVertices = {
		-.5f, -.5f, -.5f, .0f, -1.f, .0f, 0.f, 0.f,//none //0
		 .5f, -.5f, -.5f, .0f, -1.f, .0f, 1.f, 0.f,//x    //1
		-.5f, -.5f,  .5f, .0f, -1.f, .0f, 0.f, 1.f,//z    //2
		 .5f, -.5f,  .5f, .0f, -1.f, .0f, 1.f, 1.f //xz   //3
		};
		std::vector<unsigned int> squareIndices = {
			0,1,2,
			1,2,3
		};
	}
	
	Mesh::Mesh(PreMade mesh)
	{
		hasNormals = true;
		hasTexture = true;
		dim = 3;
		switch (mesh) {
		case(PreMade::Cone):
			filePath = "res/meshes/cone.obj";
			break;
		case(PreMade::Capsule):
			filePath = "res/meshes/capsule.obj";
			break;
		case(PreMade::Cube):
			filePath = "res/meshes/cube.obj";
			break;
		case(PreMade::Cylinder):
			filePath = "res/meshes/cylinder.obj";
			break;
		case(PreMade::Disk):
			filePath = "res/meshes/disk.obj";
			break;
		case(PreMade::Icosahedron):
			filePath = "res/meshes/icosahedron.obj";
			break;
		case(PreMade::Sphere):
			filePath = "res/meshes/sphere.obj";
			break;
		case(PreMade::Quartz):
			filePath = "res/meshes/quartz.obj";
		}

		if (mesh == PreMade::Square)
		{
			vertices = squareVertices;
			indices = squareIndices;
		}
		else
			loadObj(hasNormals, hasTexture);

		vb.init(&vertices[0], vertices.size() * sizeof(float));
		layout.push(GL_FLOAT, dim);
		if (hasNormals)
			layout.push(GL_FLOAT, dim);
		if (hasTexture)
			layout.push(GL_FLOAT, 2);
		vao.addBuffer(vb, layout);
		ibo.init(&indices[0], indices.size());
	}

	Mesh::Mesh(const Mesh& rhs) 
		:vertices(rhs.vertices), indices(rhs.indices), dim(rhs.dim), hasNormals(rhs.hasNormals), hasTexture(rhs.hasTexture), filePath(rhs.filePath)
	{update();}

	Mesh& Mesh::operator=(const Mesh& rhs)
	{
		vertices = (rhs.vertices);
		indices = (rhs.indices);
		dim=(rhs.dim);
		hasNormals = (rhs.hasNormals);
		hasTexture = (rhs.hasTexture);
		filePath = (rhs.filePath);
		update();
		return *this;
	}
	

	Mesh::~Mesh() {}

	void Mesh::update()
	{
		layout = VertexBufferLayout();
		vb.init(&vertices[0], vertices.size() * sizeof(float));
		layout.push(GL_FLOAT, dim);
		if (hasNormals)
			layout.push(GL_FLOAT, dim);
		if (hasTexture)
			layout.push(GL_FLOAT, 2);
		vao.addBuffer(vb, layout);
		ibo.init(&indices[0], indices.size());
	}

	void Mesh::bind() const
	{
		vao.bind();
		ibo.bind();
	}
	void Mesh::unBind() const
	{
		vao.unBind();
		ibo.unBind();
	}

	namespace {
		struct uivec2
		{
			unsigned int x, y;
		};

		struct uivec3
		{
			unsigned int x, y, z;
		};

		uivec2 get2Nums(const std::string& line)
		{
			uivec2 out;
			std::string num;
			int i;
			for (i = 0;i < line.length();i++)
			{
				if (line[i] == '/')
				{
					out.x = std::stoi(num) - 1;
					break;
				}
				else
					num += line[i];
			}
			num = "";
			for (i = i + 2;i < line.length();i++)
			{
				num += line[i];
			}
			out.y = std::stoi(num) - 1;
			return out;
		}

		uivec3 get3Nums(const std::string& line)
		{
			uivec3 out;
			std::string num;
			int i;
			for (i = 0;i < line.length();i++)
			{
				if (line[i] == '/')
				{
					out.x = std::stoi(num) - 1;
					break;
				}
				else
					num += line[i];
			}
			num = "";
			for (i = i + 1;i < line.length();i++)
			{
				if (line[i] == '/')
				{
					out.y = std::stoi(num) - 1;
					break;
				}
				else
					num += line[i];
			}
			num = "";
			for (i = i + 1;i < line.length();i++)
			{
				num += line[i];
			}
			out.z = std::stoi(num) - 1;
			return out;
		}

		int numFaces(std::string line)
		{
			int cnt = 0;
			for (char c : line)
				if (c == '/')
					cnt++;
			return cnt / 2;
		}
	}

	void Mesh::loadObj(bool hasNormals, bool hasTexture)
	{
		std::ifstream in(filePath);
		std::string line;
		std::vector<glm::vec3> positions,
			normals;
		std::vector<glm::vec2> textures;
		std::unordered_map<std::string, int> map;
		int cnt = 0;
		while (in >> line)
		{
			if (line == "vn")
			{
				float x, y, z;
				in >> x >> y >> z;
				normals.push_back(glm::vec3(x, y, z));
			}
			else if (line == "vt")
			{
				float x, y;
				in >> x >> y;
				textures.push_back(glm::vec2(x, y));
			}
			else if (line == "v")
			{
				float x, y, z;
				in >> x >> y >> z;
				positions.push_back(glm::vec3(x, y, z));
			}
			else if (line == "f")
			{
				std::getline(in, line);
				int faceVertices = numFaces(line);
				std::stringstream stream(line);

				int* tempIndices = new int[faceVertices];
				for (int i = 0;i < faceVertices;i++)
				{
					stream >> line;
					if (map.find(line) != map.end())
						tempIndices[i] = map[line];
					else
					{
						if (!hasTexture)
						{
							uivec2 nums = get2Nums(line);
							vertices.push_back(positions[nums.x].x);
							vertices.push_back(positions[nums.x].y);
							vertices.push_back(positions[nums.x].z);
							vertices.push_back(normals[nums.y].x);
							vertices.push_back(normals[nums.y].y);
							vertices.push_back(normals[nums.y].z);
						}
						else
						{
							uivec3 nums = get3Nums(line);
							vertices.push_back(positions[nums.x].x);
							vertices.push_back(positions[nums.x].y);
							vertices.push_back(positions[nums.x].z);
							vertices.push_back(normals[nums.z].x);
							vertices.push_back(normals[nums.z].y);
							vertices.push_back(normals[nums.z].z);
							vertices.push_back(textures[nums.y].x);
							vertices.push_back(textures[nums.y].y);
						}
						map[line] = cnt;
						tempIndices[i] = cnt;
						cnt++;
					}
				}
				for (int i = 1;i < faceVertices - 1;i++)
				{
					indices.push_back(tempIndices[0]);
					indices.push_back(tempIndices[i]);
					indices.push_back(tempIndices[i + 1]);
				}
				delete[] tempIndices;
			}
		}
		in.close();
	}
}