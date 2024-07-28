CXX      = g++
CXXFLAGS = -std=c++11 -Wno-unused-variable -Wno-unknown-pragmas -Wall -O3 -g -I./include -I./include/azcore -static -static-libgcc -static-libstdc++
LDFLAGS  = -lraylib -lgdi32 -lwinmm -static -static-libgcc -static-libstdc++
LIBS     = -L./lib
# $(wildcard src/*.cpp) and src/azcore/*.cpp
CXXSRC   = $(wildcard src/*.cpp)
CXXOBJ   = $(CXXSRC:.cpp=.o)
TARGET   = bin/main.exe

all: $(TARGET)

$(TARGET): $(CXXOBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(CXXOBJ) $(TARGET)

run: $(TARGET)
	$(TARGET)

rhdpi: $(TARGET)
	$(TARGET) --highdpi

azure: $(TARGET)
	$(TARGET) -m 50m