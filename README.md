### ⚠️ This is still in development ⚠️ 
### There will be a lot of missing features

# DustOS

DustOS is my attempt of making an operating system. 

A lot of code in this repository I have not written my self as I am lazy.. 

I have tweaked some files to my own liking. 

The aim of this operating system is to help me get a better understading on low level programming.

I have linked below the repo/s that I have used code from/or have taken inspiration from:

- https://github.com/FRosner/FrOS

It currently only supports:
- x86

## Contributing

If you would like to contribute to this project, please feel free to submit a pull request and I will try to merge it.

I would really appreciate any help or suggestions.

## How to run

These programs are required for the OS to run:

```
make \
nasm \
i686-elf-gcc \
i686-elf-ld \
grub-mkrescue
```

For emulation you can install either `qemu-system-i386` or `qemu-full` or just use another emulation software to run the .iso file.

You can then run:
```
git clone https://github.com/zzhjf8/DustOS && cd DustOS/
```
and either `make` to build the iso only or `make run` to build the iso and emulate it in qemu.

To cleanup you can run `make clean`
