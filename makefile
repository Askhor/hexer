compile: build
	cd build && meson compile

build:
	meson setup build

test: compile
	cat hexer.cpp | build/hexer -f

help: compile
	build/hexer -h
