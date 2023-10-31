#pragma once

#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <iostream>
#include <string>
#include <glm/glm.hpp>

namespace Audio {
#ifdef __EMSCRIPTEN__
#define ASSERT(x) 
#else
#define ASSERT(x) \
		if(!(x)) __debugbreak();
#endif

#define ALCall(x) \
		x;\
		ASSERT(ALErrorLog(__FILE__,#x,__LINE__));

	extern ALCdevice* audioDevice;
	extern ALCcontext* audioContext;

	bool init();
	void cleanUp();

	bool ALErrorLog(const char* file, const char* function, int line);
	namespace Listener
	{
		void setPosition(glm::vec3 pos);
		void setVelocity(glm::vec3 velocity);
		void setOrientation(glm::vec3 dir);
		void setGain(float gain);
	};
}