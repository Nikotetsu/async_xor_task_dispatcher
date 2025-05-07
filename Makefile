CXX = g++
CXXFLAGS = -std=c++23 -O2 -Wall -Wextra -pthread
LDFLAGS =

SRCS = main.cpp dispatcher.cpp worker_thread.cpp
OBJS = $(SRCS:.cpp=.o)
DEPS = dispatcher.h worker_thread.h lockfree_queue.h

TARGET = app

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
