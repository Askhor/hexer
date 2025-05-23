compile: build
	cd build && meson compile

build:
	meson setup build
