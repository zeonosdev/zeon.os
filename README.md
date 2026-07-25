# ZEON OS

ZEON OS is a lightweight, bare-metal operating system built directly on UEFI using C. It includes a built-in Virtual File System (RAMDisk), an interactive code editor, and framebuffer graphics.

## MAKE BY

-**MULL CORPORATION**

## Features

- **Virtual File System**: RAMDisk storage supporting basic commands (`ls`, `cat`, `touch`, `cp`, `rename`, `rm`, `df`).
- **File Explorer**: Visual file manager and storage overview via `explorer`.
- **Code Editor**: Built-in editor to write and edit text files via `edit <filename>`.
- **FUST Engine**: Framebuffer-based wallpaper rendering (`fust` and `cyber`).
- **System Shell**: Terminal environment with `neofetch`, `clear`, and `poweroff`.

## Quick Start

### 1. Install Prerequisites (Ubuntu / Debian / WSL)

```bash
sudo apt update
sudo apt install build-essential gnu-efi qemu-system-x86 genisoimage
