all:
	@mkdir -p build
	-mkdir -p bin
	cd build && cmake ..
	cd build && make

.PHONY: all clean purge

clean:
	cd build && make clean

purge:
	rm -rf build
	rm -rf bin
