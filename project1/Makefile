CC=g++
CFLAGS=-ggdb -Wall -std=c++17

all: predictors.o
	$(CC) predictors.o
predictors.o: predictors.cpp
	$(CC) $(CFLAGS) -c predictors.cpp		
clean:
	-rm -f *.o predictors.o o.txt mix
