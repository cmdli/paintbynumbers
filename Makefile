all: build
build:
	g++ -O3 -g pbn.cpp Point.cpp Image.cpp -o pbn -ljpeg -lpng -lSDL2
clean:
	rm pbn
run:
	./pbn testpng.png output.bmp
