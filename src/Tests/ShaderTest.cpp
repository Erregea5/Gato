#include <Application.h>
using namespace Application;


std::string toy = "res/shaders/toy.shader";

void shaderInterface()
{
	if (ImGui::Button("reload"))
	{
		Graphics::Shader *newShader= new Graphics::Shader(toy);
		if (newShader->successful)
		{
			delete screen->shader;
			screen->shader = newShader;
		}
	}
}

int shaderTest() 
{
	if (init(1020, 640, ""))
	{
		screen->shader = new Graphics::Shader(toy);
		userInterface = shaderInterface;
		runApp();
	}
	return 0;
}