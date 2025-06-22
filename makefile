.PHONY: all compile install remove clean

all: compile install
	
compile:
	gcc main.c -o xga-viewer `sdl2-config --cflags --libs`
	
install:
	sudo cp xga-viewer /bin

remove:
	sudo rm -f /bin/xga-viewer
	
clean:
	rm xga-viewer
