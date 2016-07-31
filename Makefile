oscilloscoper.exe: src/*.cpp src/*.hpp
	g++ -O3 -o oscilloscoper.exe src/*.cpp src/DspFilters/*.cpp  -I src -I . -I /mingw/include/sdl2 -lmingw32 -lSDL2main -lSDL2.dll -D__STDC_CONSTANT_MACROS -lavcodec -lavutil -lavformat -lavfilter -lavdevice -lswscale -s
