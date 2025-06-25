CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -I./src
SRC = $(wildcard src/asset/*.cpp src/loading/*.cpp src/sim/*.cpp)
OBJ = $(SRC:.cpp=.o)
TARGET = build/finsim/sim

all: $(TARGET)

$(TARGET): main.cpp $(OBJ) 
	mkdir -p $(dir $@) 
	$(CXX) $(CXXFLAGS) -o $@ main.cpp $(OBJ)

%.o: %.cpp 
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean: 
	rm -f $(OBJ) $(TARGET)