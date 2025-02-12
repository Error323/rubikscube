CXX = clang++
CXXFLAGS = $(shell cat compile_flags.txt)
TARGET = rubiks


all: dbg release

dbg: rubikscube.cpp
	$(CXX) -O0 -ggdb $(CXXFLAGS) -o $(TARGET)-dbg.exe rubikscube.cpp

release: rubikscube.cpp
	$(CXX) -O3 -DNDEBUG $(CXXFLAGS) -o $(TARGET).exe rubikscube.cpp

profile: rubikscube.cpp
	$(CXX) -O3 -fno-inline-functions -DNDEBUG $(CXXFLAGS) -o $(TARGET).exe rubikscube.cpp -lprofiler

clean:
	rm -f $(TARGET)-dbg.exe $(TARGET).exe

.PHONY: all clean release dbg
