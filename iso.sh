#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp sysroot/boot/bendos.kernel isodir/boot/bendos.kernel
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "bendOS" {
	multiboot /boot/bendos.kernel
}
EOF
grub-mkrescue -o bendos.iso isodir
