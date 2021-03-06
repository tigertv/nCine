#ifndef CLASS_NCINE_AUDIOLOADERWAV
#define CLASS_NCINE_AUDIOLOADERWAV

#include "IAudioLoader.h"

namespace ncine {

/// WAVE audio loader
class AudioLoaderWav : public IAudioLoader
{
  public:
	explicit AudioLoaderWav(const char *filename);
	explicit AudioLoaderWav(nctl::UniquePtr<IFile> fileHandle);

	unsigned long int read(char *buffer, unsigned long int bufferSize) const override;
	void rewind() const override;

  private:
	/// Header for the RIFF WAVE format
	struct WavHeader
	{
		char chunkId[4];
		uint32_t chunkSize;
		char format[4];

		char subchunk1Id[4];
		uint32_t subchunk1Size;
		uint16_t audioFormat;
		uint16_t numChannels;
		uint32_t sampleRate;
		uint32_t byteRate;
		uint16_t blockAlign;
		uint16_t bitsPerSample;

		char subchunk2Id[4];
		uint32_t subchunk2Size;
	};
};

}

#endif
