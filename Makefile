CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I.

SRCS = src/main.cpp src/lexer.cpp src/parser.cpp src/codegen.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = kcc

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) *.s *.o *.bin
