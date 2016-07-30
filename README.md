# oscilloscoper
Creates oscilloscope videos from WAV files.

##Usage

oscilloscoper.exe WavFile1.wav WavFile2.wav ...

##Command line options

-output <Filename>
Set output filename (default is Output.h264)

-width <Video width in pixels>
-height <Video height in pixels>
Set output stream dimensions (default is 1920x1080)

-fps <Frames per second>
Set output stream frame rate (default is 60 FPS)

-bitrate <Bits per second>
Set output stream quality (default is 8000000)

-yscale <Scale>
Set y-coordinate scaling for the following .WAV file. Can be independently set for each input .WAV file.
