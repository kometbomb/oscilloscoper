#pragma once

#include <vector>

class Wave;
class Oscilloscope;
class SDL_Renderer;

class Screen
{
	std::vector<Wave*> mWaves;
	std::vector<Oscilloscope*> mOscilloscopes;
	int mMargin;
	int mWidth, mHeight;
public:
	Screen(int width, int height);
	~Screen();
	bool addWave(const char *filename, int waveChannel, float yScale, float filterCenterFreq = -1, float filterBandwidth = -1);
	int getWaveCount() const;
	void setMargin(int margin);
	void setWidth(int width);
	void setHeight(int height);
	const float getLengthMs() const;
	void draw(float positionMs, SDL_Renderer *renderer);
};
