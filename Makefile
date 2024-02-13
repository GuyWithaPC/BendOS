
.PHONY: build clean iso run debug

build:
	./build.sh

clean:
	./clean.sh

iso:
	./iso.sh

run:
	./qemu.sh