# oscilloscoper

Creates oscilloscope videos from WAV files. Multiple files can be simultaneously rendered in the same video. Output is a raw H.264 stream. You need to use e.g. ffmpeg to mux the video stream with the audio in a playable container like MP4.

##Usage

```oscilloscoper.exe WavFile1.wav WavFile2.wav ...```

##Input files

Oscilloscoper reads normal PCM RIFF wave files. Stereo files will be reduced into mono files.

##Command line options

```-output <Filename>```
Set output filename. Default is Output.h264.

```-width <Video width in pixels>```
```-height <Video height in pixels>```
Set output stream dimensions. Default is 1920x1080.

```-fps <Frames per second>```
Set output stream frame rate. Default is 60 FPS.

```-bitrate <Bits per second>```
Set output stream bitrate. Default is 8000000.

```-yscale <Scale>```
Set y-coordinate scaling for the following .WAV file. Can be independently set for each input .WAV file that follow this option. Default is 1.0.

###Example

```oscilloscoper -output MyVideo.h264 -fps 50 -yscale 0.5 wav1.wav -yscale 2.5 wav2.wav```

This will read ```wav1.wav``` and ```wav2.wav```, scale down the first file by 50 %, 2.5x the second file scaling. The resulting video stream will use the default dimensions (1920x1080), the default bitrate (8 Mb/s). The frame rate will be 50 frames per second.
