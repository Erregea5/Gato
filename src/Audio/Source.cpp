
#include "Source.h"
#include "SoundBuffer.h"

namespace Audio {
	
	Source::Source()
		:buffer(0)
	{
		ALCall(alGenSources(1, &ID));
			
		ALCall(alSourcef(ID, AL_PITCH, 1));
		ALCall(alSourcef(ID, AL_GAIN, 1));
		ALCall(alSource3f(ID, AL_POSITION, 0, 0, 0));
		ALCall(alSource3f(ID, AL_VELOCITY, 0, 0, 0));
		ALCall(alSourcei(ID, AL_LOOPING, AL_FALSE));
	}
	Source::~Source()
	{
		ALCall(alDeleteSources(1, &ID));
	}

	void Source::playSound()
	{
		ALCall(alSourcePlay(ID));
	}

	void Source::setBuffer(SoundBuffer& buff)
	{
		buffer = &buff;
		ALCall(alSourcei(ID, AL_BUFFER, buff.getID()));
	}
	void Source::removeBuffer()
	{
		buffer = 0;
		ALCall(alSourcei(ID, AL_BUFFER, 0));
	}
	void Source::setPitch(float pitch)
	{
		ALCall(alSourcef(ID, AL_PITCH, pitch));
	}
	void Source::setGain(float gain)
	{
		ALCall(alSourcef(ID, AL_GAIN, gain));
	}
	void Source::setPosition(glm::vec3 pos)
	{
		ALCall(alSourcefv(ID, AL_POSITION, &pos.x));
	}
	void Source::setVelocity(glm::vec3 velocity)
	{
		ALCall(alSourcefv(ID, AL_VELOCITY, &velocity.x));
	}
	void Source::setLooping(bool isLooping)
	{
		ALCall(alSourcei(ID, AL_LOOPING, isLooping ? AL_TRUE : AL_FALSE));
	}
}