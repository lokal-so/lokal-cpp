
STATIC_LIB = liblokal.a
SHARED_LIB = liblokal.so
SOURCES = src/LokalSo/Lokal.cpp src/LokalSo/CurlWrapper.cpp
HEADERS = $(SOURCES:.cpp=.hpp)
OBJECTS = $(SOURCES:.cpp=.o)
INCLUDE = -I./src/

CXX = g++
CXXFLAGS = -Wall -Wextra -O2 -ggdb3 -std=c++14 -fPIC -fpic $(INCLUDE)
LDFLAGS = -O2 -fPIC -fpic
LDLIBS = -lcurl

all: $(STATIC_LIB) $(SHARED_LIB)

$(STATIC_LIB): $(OBJECTS)
	ar rcs $@ $^ 

$(SHARED_LIB): $(OBJECTS)
	$(CXX) $(LDFLAGS) -shared -o $@ $^ $(LDLIBS)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

example.o: example.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

example: example.o $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS)

clean:
	rm -f $(STATIC_LIB) $(SHARED_LIB) $(OBJECTS)

.PHONY: all clean
