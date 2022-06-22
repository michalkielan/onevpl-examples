all:
	$(CXX) -std=c++17 -Wall -O2 hello-encode.cpp -lvpl -lva-drm -lva -o hello-encode
