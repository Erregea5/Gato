#pragma once

#include "Listener.h"

namespace Audio {
	class SoundBuffer
	{
	public:
		ALuint ID;
		std::string filePath;
	public:
		SoundBuffer(const std::string& _file="");
		~SoundBuffer();

		void setSound(const std::string& _file);
		ALuint getID();
	};
}