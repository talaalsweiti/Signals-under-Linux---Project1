CC = g++
CFLAGS = -g -Wall -std=c++11

all: parent child coprocessor openglTest opgl

parent: parent.cpp
	$(CC) $(CFLAGS) parent.cpp -o parent

child: child.cpp
	$(CC) $(CFLAGS) child.cpp -o child

coprocessor: coprocessor.cpp
	$(CC) $(CFLAGS) coprocessor.cpp -o coprocessor

openglTest: openglTest.cpp
	g++ openglTest.cpp -o openglTest -lglut -lGLU -lGL
opgl: opgl.cpp
	g++ opgl.cpp -o opgl -lglut -lGLU -lGL

