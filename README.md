# oscilloscoper

Creates oscilloscope videos from WAV files. Multiple files can be simultaneously rendered in the same video. Output is a raw H.264 stream. You need to use e.g. ffmpeg to mux the video stream with the audio in a playable container like MP4.

## Usage

```
oscilloscoper.exe WavFile1.wav WavFile2.wav ...
```

### Muxing with ffmpeg

```
oscilloscoper -fps 50 Input.wav
ffmpeg -i Input.wav -r 50 -i Output.h264 -vcodec copy Output.mp4
```

## Input files

Oscilloscoper reads normal PCM RIFF wave files. Stereo files will be reduced into mono files. Oscilloscoper tries to center the current oscilloscope display to zero crossings. For more complex input (with noise etc.) you can use the filtering options to try to stabilize the display.

## Command line options

```-output <Filename>```
Set output filename. Default is Output.h264.

```-width <Video width in pixels>```
```-height <Video height in pixels>```
Set output stream dimensions. Default is 1920x1080.

```-fps <Frames per second>```
Set output stream frame rate. Default is 60 FPS.

```-bitrate <Bits per second>```
Set output stream bitrate. Default is 8000000.

### Options for input files

The following options apply to each input file specified after the option.

```-yscale <Scale>```
Set y-coordinate scaling for the following input files. Can be independently set for each input .WAV file that follow this option. Default is 1.0.

```-filter <Center frequency>```
Enable filter for the following input files and set the bandpass filter center frequency. Filter is disabled by default.

```-bandwidth <Center frequency>```
Enable filter for the following input files and set the bandpass bandwidth. Filter is disabled by default. Default value is 50 Hz.

```-nofilter ```
Disable filtering for the following input files. Filter is disabled by default.

```-channel <channel>```
Pick a single channel from the input file. Uses a mixdown of all channels by default. Note: this option is reset to the default setting after each input file.

### Example

```oscilloscoper -output MyVideo.h264 -fps 50 -yscale 0.5 wav1.wav -yscale 2.5 -channel 0 wav2.wav wav3.wav```

This will read the mixdown of ```wav1.wav```, the first channel of ```wav2.wav```, the full mixdown of ```wav3.wav```, scale down the first file to 50 % and scale the second and the third file to 250 %. The resulting video stream ```MyVideo.h264``` will use the default dimensions (1920x1080), the default bitrate (8 Mb/s). The frame rate will be 50 frames per second.
