CXX = g++
CXXFLAGS = -O3 -fopenmp -Wall
TARGET = full_brute
SRC = full_brute.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)
