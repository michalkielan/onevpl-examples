all:
	$(CXX) -std=c++17 -Wall -O2 hello_encode.cpp -lvpl -lva-drm -lva -o hello_encode
