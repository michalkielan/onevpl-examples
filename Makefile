SRC=src/*cpp
CXXFLAGS=-std=c++17 -Wall -O2
LDFLAGS=-lvpl -lva-drm -lva

all:
	$(CXX) $(CXXFLAGS) -I third-party $(SRC) $(LDFLAGS) -o hello_encode

clean:
	rm hello_encode
