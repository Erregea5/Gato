#pragma once

#include "sol/sol.hpp"

namespace Application {
	extern sol::state* lua;

	void initLua();

	extern std::string startingScript;

	class Script
	{
	public:
		std::string filePath;
		std::string name;
		bool loaded;

	public:
		Script(std::string _filePath="NewScript.lua");
		void load();
		void runStart();
		void runUpdate();
	};
}