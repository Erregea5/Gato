#pragma once
#include "Transform.h"
#include "Texture.h"
#include <vector>
#include <unordered_map>

class Object;

namespace Graphics {
	class FrameBuffer;
	class Renderer;
	class Shader;

	class Light
	{
	public:
		enum class LightType
		{
			Direct,
			Point
		};
		LightType type;

		Texture* texture;
		FrameBuffer* fbo;
		Renderer* renderer;
		Shader* depthShader;
	public:
		bool isActive;
		glm::vec3 color;
		float intensity;

		glm::mat4 directLightView;
		glm::mat4 pointLightViews[6];

		static const int width = 1280;
		static const int height = 1280;
	public:
		Light();
		Light(glm::vec3 _position, glm::vec3 _orientation, glm::vec3 _color, float _intensity, LightType _type, Shader& _depthShader);
		~Light();

		void setColor(glm::vec3 _color);
		void setIntensity(float _intensity);
		void setDepthShader(Shader& _depthShader);

		void clear();
		void internalUpdate();
		void draw(Object& o);
		void draw(std::vector<Object>& os);
		void draw(std::unordered_map<std::string,Object>& os);

		Transform& getTransform();
		Transform& getTransform() const;
	};
}