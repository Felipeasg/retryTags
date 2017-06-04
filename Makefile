CC=g++
CFLAGS=-std=c++98 -c
LDFLAGS=-lpthread -lplctag
SOURCES=main.cpp forte_sync.cpp forte_thread.cpp threadbase.cpp tag.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=tag_read

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o
	rm $(EXECUTABLE)
