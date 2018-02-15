#include "Screen.hpp"
#include "Encoder.hpp"
#include "SDL.h"
#include <stdio.h>

int main(int argc, char **argv)
{
	int width = 1920;
	int height = 1080;
	int frameRate = 60;
	float yScale = 1.0f;
	bool filterEnabled = false;
	float filterCenterFreq = 400;
	float filterBandwidth = 50;
	int waveChannel = -1;
	char filename[1000] = "Output.h264";

	Screen screen(width, height);
	Encoder encoder(width, height, frameRate);

	for (int i = 1 ; i < argc ; ++i)
	{
		const char *arg = argv[i];

		if (arg[0] != '-')
		{
			char waveChannelName[50];

			if (waveChannel == -1)
				strcpy(waveChannelName, "all channels");
			else
				snprintf(waveChannelName, sizeof(waveChannelName), "channel #%d", waveChannel);

			if (filterEnabled)
			{
				if (screen.addWave(arg, waveChannel, yScale, filterCenterFreq, filterBandwidth))
					printf("Added wave %s (%s), scaling by %.1f, filter at %.1f Hz, bandwidth %.1f Hz\n",
						arg, waveChannelName, yScale, filterCenterFreq, filterBandwidth);
				else
					printf("Could not open wave %s.\n", arg);
			}
			else
			{
				if (screen.addWave(arg, waveChannel, yScale))
					printf("Added wave %s (%s), scaling by %.1f, no filter\n", arg, waveChannelName, yScale);
				else
					printf("Could not open wave %s.\n", arg);
			}

			// Reset to default for the next input file
			waveChannel = -1;
		}
		else
		{
			if (strcmp(arg, "-yscale") == 0 && i < argc - 1)
			{
				yScale = (atof(argv[i + 1]));
				++i;
			}
			else if (strcmp(arg, "-width") == 0 && i < argc - 1)
			{
				width = (atoi(argv[i + 1]));
				screen.setWidth(width);
				++i;
			}
			else if (strcmp(arg, "-height") == 0 && i < argc - 1)
			{
				height = atoi(argv[i + 1]);
				screen.setWidth(height);
				++i;
			}
			else if (strcmp(arg, "-output") == 0 && i < argc - 1)
			{
				strncpy(filename, argv[i + 1], sizeof(filename));
				++i;
			}
			else if (strcmp(arg, "-bitrate") == 0 && i < argc - 1)
			{
				++i;
				encoder.setBitrate(atoi(argv[i + 1]));
			}
			else if (strcmp(arg, "-fps") == 0 && i < argc - 1)
			{
				frameRate = atoi(argv[i + 1]);
				encoder.setFramerate(frameRate);
				++i;
			}
			else if (strcmp(arg, "-filter") == 0 && i < argc - 1)
			{
				filterEnabled = true;
				filterCenterFreq = atof(argv[i + 1]);
				++i;
			}
			else if (strcmp(arg, "-nofilter") == 0)
			{
				filterEnabled = false;
			}
			else if (strcmp(arg, "-bandwidth") == 0 && i < argc - 1)
			{
				filterEnabled = true;
				filterBandwidth = atof(argv[i + 1]);
				++i;
			}
			else if (strcmp(arg, "-channel") == 0 && i < argc - 1)
			{
				waveChannel = atoi(argv[i + 1]);
				++i;
			}
			else
			{
				if (i >= argc - 1)
				{
					fprintf(stderr, "ERROR: Expecting value for '%s'\n", arg);
				}
				else
				{
					fprintf(stderr, "ERROR: Unknown option '%s'\n", arg);
				}
			}
		}
	}

	if (screen.getWaveCount() == 0)
	{
		fprintf(stderr, "ERROR: No waves specified on command line\n");
		return 1;
	}

	SDL_Init(SDL_INIT_VIDEO);

	SDL_Surface *surface = SDL_CreateRGBSurface(0, width, height, 32, 0xff, 0xff00, 0xff0000, 0);
	SDL_Renderer *renderer = SDL_CreateSoftwareRenderer(surface);

	encoder.initEncoder(filename);

	printf("Rendering %dx%d/%d FPS video to %s\n", width, height, frameRate, filename);

	int frame = 0;

	for (float ms = 0; ms < screen.getLengthMs() ; ms += (float)1000 / frameRate)
	{
		if ((frame % 100) == 0)
			printf("\rRendering frame %d at %d:%02d/%d:%02d", frame, (int)(ms / 1000) / 60, (int)(ms / 1000) % 60, (int)(screen.getLengthMs() / 1000) / 60, (int)(screen.getLengthMs() / 1000) % 60);

		SDL_FillRect(surface, NULL, 0);
		screen.draw(ms, renderer);
		encoder.writeFrame(surface);

		++frame;
	}

	printf("\nDone.\n");

	encoder.deinitEncoder();

	SDL_DestroyRenderer(renderer);
	SDL_FreeSurface(surface);

	return 0;
}
