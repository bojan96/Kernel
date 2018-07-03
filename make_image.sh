#!/bin/bash

cp build/os.bin boot/boot
grub-mkrescue -o boot/os.iso boot
