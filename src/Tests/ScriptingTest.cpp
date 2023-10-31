#include "Scripting.h"
#include "Application.h"
using namespace Application;

/// @TODO
/// 
/// Top todo:
/// //Make proper Imgui
/// //make proper scripting
/// 
/// 
/// 
/// test all of the scripts properties {game objects,meshes,lights, ..., imgui, and global objects}
/// 
/// add create new and change name buttons
/// 
/// add all the inspect properties for each type of game object
/// 
/// think of how to make engine extensible so people can add to it easily
/// 
/// make some sort of simple physics engine
/// 
/// actually implement and test sound system
/// 
/// create a way to publish games
/// 
/// create starting screen for editor to allow a new game or choose existing ones
/// 
/// create some menu at the top of the editor to change other things
/// 
/// maybe make a proper filesystem for game objects
/// 
/// make the editor look polished , use ImDrawList::AddImage
/// 
/// make visual studio extension to understand my lua words or any other code editor 
///// 
std::string getType(sol::type r)
{
	switch (r) {
		case sol::type::none : return "none ";
		case sol::type::lua_nil : return "lua_nil ";
		case sol::type::string : return "string";
		case sol::type::number : return "number";
		case sol::type::thread : return "thread";
		case sol::type::boolean : return "boolean";
		case sol::type::function : return "function";
		case sol::type::userdata : return "userdata";
		case sol::type::lightuserdata : return "lightuserdata";
		case sol::type::table : return "table";
		case sol::type::poly : return "poly";
	}
	return "idk";
}

void luaInterface()
{
	if (ImGui::Button("reload"))
	{
		try 
		{
			scripts["My New Script"] = Script("My New Script");
			scripts["My New Script"].load();
			auto script = (*lua)[scripts["My New Script"].name];

			sol::protected_function start = script["onStart"];
			sol::protected_function_result startResult = start.call();
			sol::protected_function update = script["onUpdate"];
			sol::protected_function_result updateResult = update.call();
			if (startResult.valid())
				std::cout << "successful start" << " returns " << getType(startResult.get_type()) << std::endl;
			else
				std::cout << "unsuccesful start" << std::endl;
			if (updateResult.valid())
				std::cout << "successful update" << " returns " << getType(updateResult.get_type()) << std::endl;
			else
				std::cout << "unsuccesful start" << std::endl;
		}
		catch (...) 
		{
			std::cout << "[Script Error]" << std::endl;
		}
	}
}

int mainm()
{
	if (!init(960, 540, ""))
		return 1;
	
    scripts.try_emplace("My New Script", "My New Script");
	userInterface = luaInterface;

	runApp();
	return 0;
}