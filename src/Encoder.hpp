#pragma once

#include <stdio.h>
class SDL_Surface;
class SwsContext;

extern "C" {
#include <libavcodec/avcodec.h>
}

class Encoder
{
	int mWidth, mHeight, mFrameRate, mBitRate;
	FILE *mOutputFile;
	AVCodecContext *mCodecContext;
	AVFrame *mFrame;
	AVPacket mPacket;
	int mFrameNumber;
	struct SwsContext * mSwsContext;
	
public:
	Encoder(int width, int height, int frameRate);
	~Encoder();
	
	void setFramerate(int frameRate);
	void setBitrate(int bitrate);
	
	void initEncoder(const char *filename);
	void deinitEncoder();
	void writeFrame(SDL_Surface *data);
};
