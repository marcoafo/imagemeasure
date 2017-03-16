all:
	g++ `pkg-config --cflags --libs opencv` measure.cpp -o measure
