# Carley Operating System (COS) & CK Kernel with K Language (`kcc`)

Welcome to **Carley Operating System (COS)** powered by **CK (Carley Kernel)**, built completely with the custom **K Programming Language** and its compiler `kcc`.

---

## About CK (Carley Kernel) & COS (Carley Operating System)

- **Kernel Name:** `CK` (*Carley Kernel*)
- **Operating System Name:** `COS` (*Carley Operating System*)
- **Architecture Target:** 32-bit x86 Bare-Metal (Multiboot 1 standard compliant)
- **UI Vision:** Customized macOS-style graphical desktop experience (Top Menu Bar, Desktop Window Containers, and Bottom Dock) running directly on bare metal.

---

## Default Hardware Drivers Included in K (`lib/`)

By default, **COS / CK** comes with a complete suite of hardware drivers built-in:

1. **`lib/input_devices.dk` / `lib/input.dk` (Input Devices):**
   - **Keyboard Driver:** PS/2 and USB keyboard scancode polling, ASCII conversion, key-wait events.
   - **Mouse Driver:** PS/2 mouse initialization, click detection, and movement tracking.
2. **`lib/hardware/hdmi.dk` (HDMI / High-Res Graphics):**
   - Linear Framebuffer (VBE / HDMI Output) supporting pixel rendering, rectangles, and screen filling.
3. **`lib/hardware/screen.dk` (VGA Screen Driver):**
   - 80x25 VGA text mode character writing and screen color clearing routines.
4. **`lib/hardware/disk.dk` (Storage / Disk Drive Driver):**
   - ATA / IDE Hard Disk controller driver for reading 512-byte sectors (`disk_read_sector`).
5. **`lib/hardware/speaker.dk` (PC Speaker Audio):**
   - PC Speaker hardware tone frequency generator (`speaker_emit_tone`).
6. **`lib/ui/window.dk` (UI Window Engine):**
   - macOS-style desktop windows, double-line box containers, and titlebars.
7. **`lib/system.dk` (Umbrella Module):**
   - Single-line import that loads all input, graphics, audio, disk, and UI drivers automatically.

---

## K Programming Language Reference Guide

### 1. File Extensions

- **`.k` Files (Main Kernel Executable Scripts):**
  Contains direct top-level statements that execute sequentially line-by-line upon machine boot. No mandatory `main()` function is required.
- **`.dk` Files (Definition & Library Header Modules):**
  Contains reusable hardware drivers, mathematical functions, and UI routines (e.g. `lib/system.dk`, `lib/hardware/hdmi.dk`).

---

### 2. Language Keywords & Syntax Rules

| Keyword / Primitive | Description | Example |
| :--- | :--- | :--- |
| `import` | Includes module definitions from `.dk` files. | `import lib/system.dk` |
| `fnc` / `fn` | Declares a named function with parameters. | `fnc add(a, b) { return a + b }` |
| `if` | Conditional branching block. | `if (x == 10) { ... }` |
| `while` | Loop statement. | `while (count < 80) { ... }` |
| `return` | Exits function and returns a value. | `return 0` |
| `mem_write(address, val)` | Writes a raw byte/word directly to physical RAM address. | `mem_write(0x000B8000, 65)` |
| `mem_read(address)` | Reads raw value directly from physical RAM address. | `val = mem_read(0x000B8000)` |
| `outb(port, val)` | Writes byte directly to hardware I/O port. | `outb(0x61, 3)` |
| `inb(port)` | Reads byte directly from hardware I/O port. | `status = inb(0x64)` |

---

## Step-by-Step K Language Code Examples

### Example 1: Full System Boot with Input, HDMI, and Disk (`cos_kernel.k`)
```k
import lib/system.dk

# 1. Fill screen with green background
screen_fill_color(47)

# 2. Draw macOS-style Window Container
ui_draw_window(10, 3, 60, 16, 31, 112, 120)

# 3. Read input event from keyboard or mouse
key_event = input_poll_event()

# 4. Read sector 0 from Disk Drive into RAM address 0x200000
disk_read_sector(0, 0x00200000)

# 5. Play startup sound chime
speaker_emit_tone(523)
```

---

## How to Build and Run COS / CK Kernel

### 1. Build the Compiler (`kcc`)
```bash
make
```

### 2. Compile `cos_kernel.k` into `kernel.bin`
```bash
./kcc examples/cos_kernel.k -o kernel.bin
```

### 3. Test in Windows PowerShell with QEMU
```powershell
& "C:\Program Files\qemu\qemu-system-i386.exe" -kernel kernel.bin
```
