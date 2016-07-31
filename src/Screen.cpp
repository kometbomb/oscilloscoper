#include "Screen.hpp"
#include "Wave.hpp"
#include "Oscilloscope.hpp"
#include "SDL.h"

Screen::Screen(int width, int height)
	:mMargin(0), mWidth(width), mHeight(height)
{
}


Screen::~Screen()
{
	for (std::vector<Oscilloscope*>::iterator i = mOscilloscopes.begin() ; i != mOscilloscopes.end() ; ++i)
		delete *i;
	
	for (std::vector<Wave*>::iterator i = mWaves.begin() ; i != mWaves.end() ; ++i)
		delete *i;
}


void Screen::addWave(const char *filename, float yScale, float filterCenterFreq, float filterBandwidth)
{
	Wave *wave = new Wave();
	wave->load(filename);
	mWaves.push_back(wave);
	
	Oscilloscope *osc = new Oscilloscope(*wave, 100, yScale);
	
	if (filterCenterFreq >= 0.0f)
		osc->setFilter(filterCenterFreq, filterBandwidth);
	
	mOscilloscopes.push_back(osc);
}

void Screen::setMargin(int margin)
{
	mMargin = margin;
}


const float Screen::getLengthMs() const
{
	float ms = 0;
	for (std::vector<Oscilloscope*>::const_iterator i = mOscilloscopes.begin() ; i != mOscilloscopes.end() ; ++i)
	{
		float l = (*i)->getWave().getLength() / ((*i)->getWave().getSampleRate() / 1000);
		if (ms < l)
			ms = l;
	}
	return ms;
}


void Screen::draw(float positionMs, SDL_Renderer *renderer)
{
	int y = 0;
	
	for (std::vector<Oscilloscope*>::const_iterator i = mOscilloscopes.begin() ; i != mOscilloscopes.end() ; ++i)
	{
		SDL_Rect area = {0, y * mHeight / mOscilloscopes.size(), mWidth, mHeight / mOscilloscopes.size()};
		(*i)->draw(positionMs * (*i)->getWave().getSampleRate() / 1000, renderer, area); 
		++y;
	}
}


void Screen::setWidth(int width)
{
	mWidth = width;
}


void Screen::setHeight(int height)
{
	mWidth = height;
}


int Screen::getWaveCount() const
{
	return mWaves.size();
}
