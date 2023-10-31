#include "SoundBuffer.h"
#include "AudioFile/AudioFile.h"

namespace Audio {
	SoundBuffer::SoundBuffer(const std::string& _file)
	{
		ALCall(alGenBuffers(1, &ID));
		if (_file != "")
			setSound(_file);
	}
	SoundBuffer::~SoundBuffer()
	{
		ALCall(alDeleteBuffers(1, &ID));
	}
	void SoundBuffer::setSound(const std::string& _file)
	{
		filePath = _file;
		AudioFile<double> audio;
		audio.load(filePath);

		int channels = audio.getNumChannels();
		int sampleRate = audio.getSampleRate();
		int bitsPerSample = audio.getBitDepth();

		ALenum format;
		if (channels == 1 && bitsPerSample == 8)
			format = AL_FORMAT_MONO8;
		else if (channels == 1 && bitsPerSample == 16)
			format = AL_FORMAT_MONO16;
		else if (channels == 2 && bitsPerSample == 8)
			format = AL_FORMAT_STEREO8;
		else if (channels == 2 && bitsPerSample == 16)
			format = AL_FORMAT_STEREO16;
		else
		{
			std::cerr
				<< "ERROR: unrecognised wave format: "
				<< channels << " channels, "
				<< bitsPerSample << " bps" << std::endl;
			return;
		}
		int nspc = audio.getNumSamplesPerChannel();
		nspc -= nspc % 4;
		ALCall(alBufferData(ID, format, &audio.samples[0][0], nspc, sampleRate));
	}
	ALuint SoundBuffer::getID()
	{
		return ID;
	}
}