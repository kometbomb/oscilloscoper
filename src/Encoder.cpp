#include "Encoder.hpp"
#include "SDL.h"

extern "C" {
#include <libswscale/swscale.h>
#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
#include <libavutil/channel_layout.h>
#include <libavutil/common.h>
#include <libavutil/imgutils.h>
#include <libavutil/mathematics.h>
#include <libavutil/samplefmt.h>
}

Encoder::Encoder(int width, int height, int frameRate)
	: mWidth(width), mHeight(height), mFrameRate(frameRate), 
	mBitRate(8000000), mOutputFile(NULL)
{
	avcodec_register_all();
}


Encoder::~Encoder()
{
}

	
void Encoder::writeFrame(SDL_Surface *data)
{
	
	
	uint8_t * inData[1] = { (Uint8*)data->pixels }; // RGB24 have one plane
	int inLinesize[1] = { data->pitch }; // RGB stride
	
	int i, ret, x, y, got_output;
	
	av_init_packet(&mPacket);

	mPacket.data = NULL;    // packet data will be allocated by the encoder
	mPacket.size = 0;
	sws_scale(mSwsContext, inData, inLinesize, 0, mHeight, mFrame->data, mFrame->linesize);
	
	mFrame->pts = mFrameNumber;
	/* encode the image */
	ret = avcodec_encode_video2(mCodecContext, &mPacket, mFrame, &got_output);
	if (ret < 0) {
		fprintf(stderr, "ERROR: Error encoding frame\n");
		exit(1);
	}
	if (got_output) {
		fwrite(mPacket.data, 1, mPacket.size, mOutputFile);
		//av_packet_unref(&mPacket);
	}
	
	mFrameNumber++;
}


void Encoder::initEncoder(const char *filename)
{
	mFrameNumber = 0;
	
	mSwsContext = sws_getContext(mWidth, mHeight, AV_PIX_FMT_RGB32, mWidth, mHeight, AV_PIX_FMT_YUV420P, 0, 0, 0, 0);
		
    AVCodec *codec;
    printf("Encode video file %s\n", filename);
	
	AVCodecID codec_id = AV_CODEC_ID_H264;
	
    /* find the mpeg1 video encoder */
    codec = avcodec_find_encoder(codec_id);
    if (!codec) {
        fprintf(stderr, "ERROR: Codec not found\n");
        exit(1);
    }
    mCodecContext = avcodec_alloc_context3(codec);
    if (!mCodecContext) {
        fprintf(stderr, "ERROR: Could not allocate video codec context\n");
        exit(1);
    }
	
	mCodecContext->bit_rate = mBitRate;
    /* resolution must be a multiple of two */
    mCodecContext->width = mWidth;
    mCodecContext->height = mHeight;
    /* frames per second */
    mCodecContext->time_base = (AVRational){1,mFrameRate};
    mCodecContext->gop_size = 10; /* emit one intra frame every ten frames */
    mCodecContext->max_b_frames = 1;
    mCodecContext->pix_fmt = AV_PIX_FMT_YUV420P;
	
	if (codec_id == AV_CODEC_ID_H264)
        av_opt_set(mCodecContext->priv_data, "preset", "slow", 0);
    /* open it */
    if (avcodec_open2(mCodecContext, codec, NULL) < 0) {
        fprintf(stderr, "ERROR: Could not open codec\n");
        exit(1);
    }
    mOutputFile = fopen(filename, "wb");
    if (!mOutputFile) {
        fprintf(stderr, "ERROR: Could not open %s\n", filename);
        exit(1);
    }
    mFrame = av_frame_alloc();
    if (!mFrame) {
        fprintf(stderr, "ERROR: Could not allocate video frame\n");
        exit(1);
    }
    mFrame->format = mCodecContext->pix_fmt;
    mFrame->width  = mCodecContext->width;
    mFrame->height = mCodecContext->height;
    /* the image can be allocated by any means and av_image_alloc() is
     * just the most convenient way if av_malloc() is to be used */
    int ret = av_image_alloc(mFrame->data, mFrame->linesize, mCodecContext->width, mCodecContext->height,
                         mCodecContext->pix_fmt, 32);
    if (ret < 0) {
        fprintf(stderr, "ERROR: Could not allocate raw picture buffer\n");
        exit(1);
    }
	
	mOutputFile = fopen(filename, "wb");
}


void Encoder::deinitEncoder()
{
	int got_output;
	    /* get the delayed frames */
    for (got_output = 1; got_output; mFrameNumber++) {
        int ret = avcodec_encode_video2(mCodecContext, &mPacket, NULL, &got_output);
        if (ret < 0) {
            fprintf(stderr, "ERROR: Error encoding frame\n");
            exit(1);
        }
        if (got_output) {
            fwrite(mPacket.data, 1, mPacket.size, mOutputFile);
            av_packet_unref(&mPacket);
        }
    }
    
	avcodec_close(mCodecContext);
    av_free(mCodecContext);
    av_freep(&mFrame->data[0]);
    av_frame_free(&mFrame);
	
	sws_freeContext(mSwsContext);
	
	if (mOutputFile != NULL)
		fclose(mOutputFile);
}


void Encoder::setFramerate(int frameRate)
{
	mFrameRate = frameRate;
	
}


void Encoder::setBitrate(int bitrate)
{
	mBitRate = bitrate;
}
