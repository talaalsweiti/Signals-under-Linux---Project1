CC = g++ # specify the used compiler
CFLAGS = -Wall # specify the options added on compile.
# -Wall -> allow all warnings

all: parent child coprocessor opgl

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

