### ⚠️ This is still in development ⚠️ 
### There will be a lot of missing features

# DustOS

DustOS is my own operating system I am currently developing.

It currently only supports:
- x86

It uses the GRUB bootloader for now, though development of a custom bootloader will be implemented in the future.

The kernel should be structured similarly to how the Linux kernel source tree looks like.

I apologise for the lack of documentation and any poorly written code.

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
