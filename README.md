# BendOS
This is a small OS development project I've been messing around with.
(the name comes from some dumb ASCII art I printed out with an earlier version).
I've been working on this on and off to learn a bit of low-level programming and operating systems knowledge.
## Building
In order to run `make build`, this project requires an i386 ELF cross-compiler as described on the [OSDev Wiki](https://wiki.osdev.org/GCC_Cross-Compiler).
Once you have a cross-compiler ready, create a script in the root directory called `env.sh` with the following contents:
```bash
export PREFIX="[the directory containing your cross-compiler]"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"
```
Then, running `make build` will create the kernel binary, and `make iso` will create a bootable iso file.
More build configuration options are available to change in `config.sh`.
## Running
If you have QEMU installed on your system, running `make run` will build and run using QEMU.
