#pragma once

class Wave
{
	struct Chunk
	{
		char ckID[4];
		long cksize;
	};

	int *mSamples;
	int mLength, mSampleRate;
public:
	Wave();
	~Wave();
	bool load(const char *filename, int channel = -1);

	int getLength() const;
	int getSampleRate() const;
	int get(int samplePosition) const;
};
