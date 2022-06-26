all:
	$(CXX) -std=c++17 -Wall -I third-party -O2 hello_encode.cpp -lvpl -lva-drm -lva -o hello_encode
