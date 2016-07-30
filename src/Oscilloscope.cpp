#include "Oscilloscope.hpp"
#include "Wave.hpp"
#include "SDL.h"
#include <algorithm>

Oscilloscope::Oscilloscope(const Wave& wave, int windowSizeMs, float yScale)
	: mWave(wave), mResolution(0), mYScale(yScale)
{
	mWindowSize = (float)wave.getSampleRate() * windowSizeMs / 1000;
	mResolution = mWindowSize;
	mFilterBuffer = new float[mWindowSize * 2];
	mFilter.setup(3, mWave.getSampleRate(), 300, 200, 1);
}


Oscilloscope::~Oscilloscope()
{
	delete [] mFilterBuffer;
}


void Oscilloscope::draw(int position, SDL_Renderer *renderer, const SDL_Rect& area) 
{
	int maxLookback = mWindowSize;
	int lookback = 0;
	int bufferPosition = mWindowSize;

	if (false)
	{
		for (int i = 0 ; i < mWindowSize * 2 ; ++i)
			mFilterBuffer[i] = mWave.get(position + i - mResolution / 2);
		
		mFilter.process(mWindowSize * 2, &mFilterBuffer);
		
		float orig = mFilterBuffer[mWindowSize];
		
		while (lookback < maxLookback - 1)
		{
			if (orig > 0)
			{
				--position;
				--bufferPosition;
				if (mFilterBuffer[bufferPosition] < 0)
					break;
			}
			else 	
			{
				++position;
				++bufferPosition;
				if (mFilterBuffer[bufferPosition] > 0)
					break;
			}
			
			lookback++;
		}
	}
	else
	{
		int orig = mWave.get(position);
		
		while (lookback < maxLookback)
		{
			if (orig > 0)
			{
				--position;
				if (mWave.get(position) < 0)
					break;
			}
			else 	
			{
				++position;
				if (mWave.get(position) > 0)
					break;
			}
			
			lookback++;
		}
	}
	
	int amp, prevAmp = mWave.get(position - mResolution / 2) * mYScale * (area.h / 2) / 32768 + area.h / 2;
	
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	
	for (int x = 0 ; x < area.w ; ++x)
	{
		//amp = mFilterBuffer[x * mResolution / area.w + bufferPosition - mResolution / 2] * mYScale * (area.h / 2) / 32768 + area.h / 2; 
		amp = mWave.get(position - mResolution / 2 + x * mResolution / area.w) * mYScale * (area.h / 2) / 32768 + area.h / 2;
		
		SDL_Rect rect = {x + area.x, std::min(prevAmp, amp) - 4 + area.y, 2, abs(prevAmp - amp) + 8};
		
		SDL_RenderFillRect(renderer, &rect);
		//SDL_RenderDrawLine(renderer, x - 1 + area.x, prevAmp + area.y, x + area.x, amp + area.y);
		
		prevAmp = amp;
	}
}


const Wave& Oscilloscope::getWave() const
{
	return mWave;
}
