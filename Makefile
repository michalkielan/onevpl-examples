all:
	$(CXX) -std=c++17 -Wall -I third-party -O2 src/main.cpp src/mapping.cpp -lvpl -lva-drm -lva -o hello_encode
