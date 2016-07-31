#pragma once

class Wave;
class SDL_Renderer;
class SDL_Rect;

#include "DSPFilters/Filter.h"
#include "DSPFilters/ChebyshevI.h"

class Oscilloscope
{
	const Wave& mWave;
	int mResolution;
	int mWindowSize;
	float *mFilterBuffer;
	float mYScale;
	bool mFilterEnabled;
	float mFilterCenterFreq, mFilterBandwidth;
	Dsp::SimpleFilter<Dsp::ChebyshevI::BandPass<3>,1> mFilter;
	
public:
	Oscilloscope(const Wave& wave, int windowSizeMs, float yScale);
	~Oscilloscope();
	void setFilter(float centerFreq, float bandwidth);
	const Wave& getWave() const;
	void draw(int position, SDL_Renderer *renderer, const SDL_Rect& area);
};
