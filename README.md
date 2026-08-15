# Carley Operating System (COS) & CK Kernel with K Language (`kcc`)

Welcome to **Carley Operating System (COS)** powered by **CK (Carley Kernel)**, built completely with the custom **K Programming Language** and its compiler `kcc`.

---

## About CK (Carley Kernel) & COS (Carley Operating System)

- **Kernel Name:** `CK` (*Carley Kernel*)
- **Operating System Name:** `COS` (*Carley Operating System*)
- **Architecture Target:** 32-bit x86 Bare-Metal (Multiboot 1 standard compliant)
- **UI Vision:** Customized macOS-style graphical desktop experience (Top Menu Bar, Desktop Window Containers, and Bottom Dock) running directly on bare metal.

---

## K Programming Language Reference Guide

The K language is specifically crafted for kernel construction, OS design, and hardware programming.

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

### 3. Standard Drivers & Libraries Provided (`lib/`)

- **`lib/hardware/hdmi.dk`**: HDMI / VBE High-Resolution Framebuffer Driver (`hdmi_draw_pixel`, `hdmi_fill_screen`, `hdmi_draw_rect`).
- **`lib/hardware/screen.dk`**: VGA Text Mode Screen Driver (`screen_write_char`, `screen_fill_color`).
- **`lib/ui/window.dk`**: macOS / Desktop Window UI Engine (`ui_draw_box`, `ui_draw_window`).
- **`lib/input.dk`**: PS/2 Keyboard Input polling & ASCII translator (`input_poll_scancode`, `input_wait_key`, `input_scancode_to_ascii`).
- **`lib/hardware/speaker.dk`**: PC Speaker sound generator (`speaker_emit_tone`, `speaker_stop`).
- **`lib/hardware/mouse.dk`**: PS/2 Mouse hardware initializer (`mouse_init`).
- **`lib/system.dk`**: Umbrella library importing all hardware, HDMI, input, and UI sub-modules in one statement.

---

## Step-by-Step K Language Code Examples

### Example 1: HDMI Graphics Framebuffer Example (`hdmi_demo.k`)
```k
import lib/system.dk

# Address for linear framebuffer (VBE / HDMI Output)
fb = 0xFD000000

# Fill entire HDMI 1024x768 display with dark blue background
hdmi_fill_screen(fb, 1024, 768, 0x000033)

# Draw macOS-style Window Rectangle (x=200, y=150, w=620, h=400)
hdmi_draw_rect(fb, 1024, 200, 150, 620, 400, 0xE0E0E0)
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
