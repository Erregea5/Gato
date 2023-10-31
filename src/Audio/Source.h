#pragma once

#include "Listener.h"


namespace Audio {
	class SoundBuffer;

	class Source
	{
	private:
		ALuint ID;
	public:
		SoundBuffer* buffer;
	public:
		Source();
		~Source();


		void playSound();

		void setBuffer(SoundBuffer& buff);
		void removeBuffer();
		void setPitch(float pitch);
		void setGain(float gain);
		void setPosition(glm::vec3 pos);
		void setVelocity(glm::vec3 velocity);
		void setLooping(bool isLooping);
	};
}