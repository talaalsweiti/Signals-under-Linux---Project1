CC = g++
CFLAGS = -Wall -std=c++11

all: parent child coprocessor openglTest

parent: parent.cpp
	$(CC) $(CFLAGS) parent.cpp -o parent

child: child.cpp
	$(CC) $(CFLAGS) child.cpp -o child

coprocessor: coprocessor.cpp
	$(CC) $(CFLAGS) coprocessor.cpp -o coprocessor

openglTest: openglTest.cpp
	g++ openglTest.cpp -o openglTest -lglut -lGLU -lGL

