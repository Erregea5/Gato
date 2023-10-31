#include "Light.h"
#include "Renderer.h"

#define transform renderer->camera.transform

namespace Graphics {
	Light::Light(glm::vec3 _position, glm::vec3 _orientation, glm::vec3 _color, float _intensity, LightType _type, Shader& _depthShader)
		:color(_color), intensity(_intensity), type(_type), depthShader(&_depthShader), isActive(1)
	{
		texture = new Texture;
		if (type == LightType::Direct)
		{
			fbo = new FrameBuffer(0, *texture, width, height, false);
			renderer = new Renderer(*fbo, 45.f);
		}
		else
		{
			texture->isCube = true;
			fbo = new FrameBuffer(0, *texture, width, height, true);
			renderer = new Renderer(*fbo, 90.f);
		}
		transform.position = _position;
		transform.orientation = _orientation;
	}
	Light::Light()
		:color(glm::vec3(1)), intensity(1), type(LightType::Direct), isActive(1)
	{
		texture = new Texture;
		fbo = new FrameBuffer(0, *texture, width, height, false);
		renderer = new Renderer(*fbo, 45.f);
	}

	Light::~Light()
	{
		delete texture;
		delete renderer;
		delete fbo;
	}

	void Light::setColor(glm::vec3 _color)
	{
		color = _color;
	}
	void Light::setIntensity(float _intensity)
	{
		intensity = _intensity;
	}
	void Light::setDepthShader(Shader& _depthShader)
	{
		depthShader = &_depthShader;
	}
	
	void Light::clear()
	{
		renderer->clear();
	}
	void Light::internalUpdate()
	{
		if (type == LightType::Direct)
			directLightView = renderer->ortho * transform.view();
		else
		{
			glm::vec3& position = transform.position,
				&orientation = transform.orientation;
			pointLightViews[0] = renderer->persp *
				glm::lookAt(position, position + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
			pointLightViews[1] = renderer->persp *
				glm::lookAt(position, position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
			pointLightViews[2] = renderer->persp *
				glm::lookAt(position, position + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0));
			pointLightViews[3] = renderer->persp *
				glm::lookAt(position, position + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0));
			pointLightViews[4] = renderer->persp *
				glm::lookAt(position, position + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0));
			pointLightViews[5] = renderer->persp *
				glm::lookAt(position, position + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0));
			depthShader->bind();
			depthShader->setMatrix("u_shadowMatrices", pointLightViews[0], 6);
			depthShader->setFloat("u_lightPos", position.x, position.y, position.z);
			depthShader->setFloat("u_farPlane", renderer->farPlane);
		}
	}
	void Light::draw(Object& o)
	{
		if (!isActive)
			return;
		Shader* shader = o.shader;
		o.setShader(*depthShader);
		internalUpdate();
		if (type == LightType::Direct)
			renderer->draw2D(o);
		else
			renderer->draw(o);
		o.setShader(*shader);
	}
	void Light::draw(std::vector<Object>& os)
	{
		if (!isActive)
			return;
		std::vector<Shader*> shaders;
		for (Object& o : os)
		{
			shaders.push_back(o.shader);
			o.setShader(*depthShader);
		}
		internalUpdate();
		if (type == LightType::Direct)
			renderer->draw2D(os);
		else
			renderer->draw(os);
		for (int i = 0;i < os.size();i++)
			os[i].setShader(*shaders[i]);
	}
	void Light::draw(std::unordered_map<std::string, Object>& os)
	{
		if (!isActive)
			return;
		internalUpdate();
		for (auto& o : os)
		{
			if (o.second.hasShadow)
			{
				Shader* oShader = o.second.shader;
				o.second.setShader(*depthShader);
				if (type == LightType::Direct)
					renderer->draw2D(o.second);
				else
					renderer->draw(o.second);
				o.second.setShader(*oShader);
			}	
		}
	}

	Transform& Light::getTransform()
	{ 
		return transform; 
	}
	Transform& Light::getTransform() const
	{
		return transform;
	}
}
#undef transform