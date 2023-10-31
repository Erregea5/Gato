#include "Listener.h"

namespace Audio {
	ALCdevice* audioDevice;
	ALCcontext* audioContext;

	bool init()
	{
		audioDevice = alcOpenDevice(0);
		if (!audioDevice)
		{
			std::cerr << "OpenAL init Error: \n" << alGetError();
			return 0;
		}
		audioContext = alcCreateContext(audioDevice, 0);
		if (!alcMakeContextCurrent(audioContext))
		{
			std::cerr << "OpenAL init Error: \n" << alcGetError(audioDevice);
			return 0;
		}
		return 1;
	}
	void cleanUp()
	{
		alcMakeContextCurrent(0);
		alcDestroyContext(audioContext);
		alcCloseDevice(audioDevice);
	}

	bool ALErrorLog(const char* file, const char* function, int line)
	{
		if (ALenum err = alGetError())
		{
			std::cout << "[OpenAL Error] " << err << "\n"
				<< "in file: " << file << "\n"
				<< "in function: " << function << "\n"
				<< "at line: " << line << std::endl;
			return 0;
		}
		return 1;
	}
	
	namespace Listener
	{
		void setPosition(glm::vec3 pos)
		{
			ALCall(alListenerfv(AL_POSITION, &pos.x));
		}
		void setVelocity(glm::vec3 velocity)
		{
			ALCall(alListenerfv(AL_VELOCITY, &velocity.x));
		}
		void setOrientation(glm::vec3 dir)
		{
			float ori[6] = { dir.x,dir.y,dir.z,0.0f,1.0f,0.0f };
			ALCall(alListenerfv(AL_ORIENTATION, ori));
		}
		void setGain(float gain)
		{
			ALCall(alListenerf(AL_GAIN, gain));
		}
	}
}
