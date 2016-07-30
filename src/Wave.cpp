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


bool Wave::load(const char *filename)
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
	
	fseek(f, beginning_of_WAVE + WAVE.c.cksize + 8, SEEK_SET);
	
	Chunk peek = { "", 0 };
	
	fread(&peek, 1, sizeof(peek) ,f);
	
	if (strncmp(peek.ckID, "fact", 4) == 0)
	{
		fseek(f, sizeof(long), SEEK_CUR);
	}
	else
	{
		fseek(f, beginning_of_WAVE + WAVE.c.cksize + 8, SEEK_SET);
	}
	
	fread(&peek, 1, sizeof(peek) ,f);
	
	if (strncmp(peek.ckID, "data", 4) != 0)
	{
		fclose(f);
		return false;
	}
	
	mLength = peek.cksize / (WAVE.wBitsPerSample / 8) / WAVE.nChannels;
	mSampleRate = WAVE.nSamplesPerSec;
	
	int channels = WAVE.nChannels;
	int bitsPerSample = WAVE.wBitsPerSample;
	
	mSamples = new int[mLength];
	
	const int bufferSize = 65536;
	
	short *buffer = new short[bufferSize * channels];
	int totalRead = 0;
	
	while (totalRead < mLength)
	{
		int readCount = fread(buffer, (bitsPerSample / 8 * channels), bufferSize, f);
		
		for (int i = 0 ; i < readCount ; ++i)
		{	
			if (channels == 1)
			{
				mSamples[totalRead + i] = buffer[i * channels];
			}
			else
			{
				mSamples[totalRead + i] = buffer[i * channels] + buffer[i * channels + 1];
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
