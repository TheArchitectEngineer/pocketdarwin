CXX := clang++
CXXFLAGS := -Wall -O2 -Wextra

SRC := clang.1.cpp
TARGET := cc

prefix ?= /usr/local/bin

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

clean:
	rm $(TARGET)

install:
	install -v $(TARGET) $(prefix)/$(TARGET)

