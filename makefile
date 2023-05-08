CC = g++ # specify the used compiler
CFLAGS = -g -Wall -std=c++11 # specify the options added on compile.
# -g -> create gdb components for debugging
# -Wall -> allow all warnings
# -std=c++11 -> use the C++11 language standard

all: parent child coprocessor openglTest opgl

# compile parent.cpp
parent: parent.cpp
	$(CC) $(CFLAGS) parent.cpp -o parent

# compile child.cpp
child: child.cpp
	$(CC) $(CFLAGS) child.cpp -o child

# compile coprocessor.cpp
coprocessor: coprocessor.cpp
	$(CC) $(CFLAGS) coprocessor.cpp -o coprocessor

#compile opgl.cpp
opgl: opgl.cpp
	$(CC) opgl.cpp -o opgl -lglut -lGLU -lGL

