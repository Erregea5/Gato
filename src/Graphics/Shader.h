#pragma once
#include <string>
#include <unordered_map>
#include "Light.h"
#include "Texture.h"
#include "glm/glm.hpp"

namespace Graphics {
	struct ShaderSource {
		std::string VertexShader,
			FragmentShader,
			GeometryShader;
	};

	class Shader
	{
	private:
		int textureCount;
		unsigned int ID;
		std::unordered_map<std::string, int> uniformLocationCache;
		std::unordered_map<std::string, int> textureCache;

	public:
		std::string filePath;
		bool successful;

	public:
		Shader(std::string _filePath);
		Shader();
		Shader(const Shader& rhs);
		Shader& operator=(const Shader& rhs);
		~Shader();

		void compile();

		void bind() const;
		void unBind() const;

		void setInteger(std::string name, int v0);
		void setFloat(std::string name, float v0);
		void setFloat(std::string name, float v0, float v1, float v2);
		void setFloat(std::string name, float v0, float v1, float v2, float v3);
		void setMatrix(std::string name, const glm::mat4x4& v0, int n = 1);
		void setLight(const Light& o);
		void setTexture(std::string name, const Texture& texture);

		static std::vector<Shader> createShaders(int n, std::string filePath);
	private:
		int getUniformLocation(std::string& name);
		ShaderSource loadShaders();
		unsigned int compileShader(unsigned int type, std::string& shaderCode);
		unsigned int createShaderProgram(ShaderSource source);
	};
}