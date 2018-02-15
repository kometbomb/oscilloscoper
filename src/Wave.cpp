#include "Wave.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

enum
{
	WAVE_FORMAT_PCM = 1,
	WAVE_FORMAT_FLOAT = 3
};

Wave::Wave()
	: mSamples(NULL), mLength(0), mSampleRate(44100)
{
}


Wave::~Wave()
{
	if (mSamples != NULL)
		delete [] mSamples;
}


bool Wave::load(const char *filename, int channel)
{
	FILE *f = fopen(filename, "rb");

	if (!f)
		return false;

	struct {
		Chunk c;
		char WAVEID[4];
	}  __attribute__((__packed__)) RIFF;

	struct
	{
		Chunk c;
		short wFormatTag;
		short nChannels;
		long nSamplesPerSec;
		long nAvgBytesPerSec;
		short nBlockAlign;
		short wBitsPerSample;
		/*----*/
		short cbSize;
		/*----*/
		short wValidBitsPerSample;
		long dwChannelMask;
		char SubFormat[16];
	} __attribute__((__packed__)) WAVE;

	if (fread(&RIFF, 1 , sizeof(RIFF), f) != sizeof(RIFF) || strncmp(RIFF.c.ckID, "RIFF", 4) != 0 || strncmp(RIFF.WAVEID, "WAVE", 4) != 0)
	{
		fclose(f);
		return false;
	}

	size_t beginning_of_WAVE = ftell(f);

	if (fread(&WAVE, 1 , sizeof(WAVE), f) < 16 || strncmp(WAVE.c.ckID, "fmt ", 4) != 0)
	{
		fclose(f);
		return false;
	}

	if (WAVE.wFormatTag != WAVE_FORMAT_PCM/* && WAVE.wFormatTag != WAVE_FORMAT_FLOAT*/)
	{
		fclose(f);
		return false;
	}

	int nChannels = WAVE.nChannels;
	int bitsPerSample = WAVE.wBitsPerSample;

	if (channel >= nChannels)
	{
		fclose(f);
		return false;
	}

	fseek(f, beginning_of_WAVE + WAVE.c.cksize + 8, SEEK_SET);

	Chunk peek = { "", 0 };

	// Find "data" chunk while skipping all other types

	do
	{
		int beginningOfChunk = ftell(f);
		if (fread(&peek, 1, sizeof(peek), f) != sizeof(peek))
		{
			fclose(f);
			return false;
		}

		if (strncmp(peek.ckID, "data", 4) == 0)
			break;

		fseek(f, beginningOfChunk + peek.cksize + 8, SEEK_SET);
	}
	while (strncmp(peek.ckID, "data", 4) != 0);

	mLength = peek.cksize / (WAVE.wBitsPerSample / 8) / WAVE.nChannels;
	mSampleRate = WAVE.nSamplesPerSec;

	mSamples = new int[mLength];

	const int bufferSize = 65536;

	short *buffer = new short[bufferSize * nChannels];
	int totalRead = 0;

	while (totalRead < mLength)
	{
		int readCount = fread(buffer, (bitsPerSample / 8 * nChannels), bufferSize, f);

		for (int i = 0 ; i < readCount ; ++i)
		{
			if (nChannels == 1)
			{
				mSamples[totalRead + i] = buffer[i * nChannels];
			}
			if (channel >= 0 && channel < nChannels)
			{
				mSamples[totalRead + i] = buffer[i * nChannels + channel];
			}
			else
			{
				// Mixdown
				int total = 0;
				for (int c = 0 ; c < nChannels ; ++c)
					total += buffer[i * nChannels + c];
				mSamples[totalRead + i] = total;
			}
		}

		totalRead += readCount;
	}

	delete [] buffer;

	fclose(f);

	return true;
}


int Wave::getLength() const
{
	return mLength;
}


int Wave::getSampleRate() const
{
	return mSampleRate;
}



int Wave::get(int samplePosition) const
{
	if (samplePosition >= mLength || samplePosition < 0)
		return 0;

	return mSamples[samplePosition];
}
