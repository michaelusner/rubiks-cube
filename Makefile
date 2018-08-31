rubik: main.cpp cube.cpp
	gcc *.cpp -I/usr/include/libdrm -lGL -lm -lglut -lGLU -o rubik
