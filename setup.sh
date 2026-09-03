#!/bin/bash

echo "Detecting operating system..."
OS="$(uname -s)"

if [ "$OS" == "Darwin" ]; then
    echo "macOS detected. Installing via Homebrew..."
    if ! command -v brew &> /dev/null; then
        echo "Error: Homebrew not found. Please install it from https://brew.sh/"
        exit 1
    fi
    brew update
    brew install riscv64-elf-gcc qemu make

elif [ "$OS" == "Linux" ]; then
    if [ -f /etc/debian_version ]; then
        echo "Debian/Ubuntu detected. Installing via apt..."
        sudo apt-get update
        sudo apt-get install -y gcc-riscv64-linux-gnu binutils-riscv64-linux-gnu qemu-system-misc make
    
    elif [ -f /etc/fedora-release ]; then
        echo "Fedora detected. Installing via dnf..."
        sudo dnf install -y gcc-riscv64-linux-gnu binutils-riscv64-linux-gnu qemu-system-riscv make
    
    elif [ -f /etc/arch-release ]; then
        echo "Arch Linux detected. Installing via pacman..."
        sudo pacman -S --noconfirm riscv64-linux-gnu-gcc riscv64-linux-gnu-binutils qemu-system-riscv make
    
    else
        echo "Unsupported Linux distribution."
        echo "Please install the RISC-V GNU toolchain and QEMU manually using your package manager."
        exit 1
    fi
else
    echo "Unsupported OS: $OS"
    echo "Please check the README for manual setup instructions."
    exit 1
fi

echo ""
echo "All dependencies installed successfully!"
echo "To build and boot the OS, run:"
echo "cd xv6-riscv"
echo "make qemu"