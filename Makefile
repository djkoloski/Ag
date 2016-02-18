all: win linux

win: main.cpp src
	i586-mingw32msvc-g++ main.cpp -lgdi32 -lopengl32 -ansi -o main.exe

linux: main.cpp src
	g++ main.cpp -lX11 -lGL -ansi -o main
