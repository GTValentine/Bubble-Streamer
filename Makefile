# Convenience makefile

build:
	mkdir -p build && cd build && qmake .. && make

clean:
	rm -rf build

run: | build
	build/277

.PHONY: build clean run
