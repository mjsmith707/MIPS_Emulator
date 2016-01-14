CC=g++
CFLAGS=-std=c++11 -Ofast -march=native -mtune=native -c -Wall
LDFLAGS=-lpthread -lncurses
SOURCES=main.cpp CPU.cpp PMMU.cpp UART8250.cpp Coprocessor0.cpp COP0Register.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=mips.x

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(OBJECTS)
	rm -rf $(EXECUTABLE)
