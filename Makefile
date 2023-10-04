OUTPUT = main

CC = g++

CFLAGS = -Wall -std=c++11

LDFLAGS = -lglut -lGL -lGLU

SOURCES = main.cpp

all: $(OUTPUT)

$(OUTPUT): $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(OUTPUT) $(LDFLAGS)

clean:
	rm -f $(OUTPUT)
