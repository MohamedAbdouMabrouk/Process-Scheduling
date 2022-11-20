CC = g++ -g

all: build


build: uniprocessor.o
	$(CC) -o lab4 uniprocessor.o
	
uniprocessor.o: uniprocessor.cc uniprocessor.h
