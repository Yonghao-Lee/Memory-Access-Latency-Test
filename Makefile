CC=g++
CXX=g++
CFLAGS=-std=c++11 -O3 -Wall
CXXFLAGS=-std=c++11 -O3 -Wall

# Source files
SRCS=memory_latency.cpp measure.cpp
OBJS=$(SRCS:.cpp=.o)

# Target executable
TARGET=memory_latency

# Files to include in tar
TARSRCS=memory_latency.cpp Makefile README results.png lscpu.png page_size.png

# Tar settings
TAR=tar
TARFLAGS=-cvf
TARNAME=ex1.tar

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) $(TARGET) $(OBJS) *~ *core

# Create tar file with all required submissions
tar: $(TARSRCS)
	COPYFILE_DISABLE=1 $(TAR) $(TARFLAGS) $(TARNAME) $(TARSRCS)

# Run with default parameters (example)
run:
	./$(TARGET) 6000000000 1.1 100000000

.PHONY: all clean tar run