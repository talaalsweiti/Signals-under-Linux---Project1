CC = g++
CFLAGS = -Wall -std=c++11

all: parent child

parent: parent.o
	$(CC) $(CFLAGS) parent.o -o parent

child: child.o
	$(CC) $(CFLAGS) child.o -o child

parent.o: parent.cpp
	$(CC) $(CFLAGS) -c parent.cpp

child.o: child.cpp
	$(CC) $(CFLAGS) -c child.cpp

clean:
	rm -f *.o parent child

