CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++14 -O3

TARGET = hstp
SRCS = main.cpp cromosoma.cpp archivos.cpp algoritmo.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)