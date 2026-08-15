# project-K Language & Compiler (`kcc`)

`project-K` is a standalone programming language and compiler (`kcc`) written in C++17 specifically designed for bare-metal software, operating system kernels, and low-level driver development without requiring external build tools for K projects.

---

## Key Features

1. **Bare-Metal & Multiboot 1 Compliant:**
   - Generates executable binaries with built-in Multiboot headers that boot directly on bare metal hardware or hypervisors like QEMU.
2. **Direct Instruction Execution:**
   - Executable statements outside functions run sequentially line-by-line upon boot. Semicolons `;` are optional.
3. **Modular Definition Files (`.dk`):**
   - Import functions, constants, and drivers using `import filename.dk`. Umbrella files (e.g. `lib/system.dk`) can import other `.dk` files to aggregate entire hardware subsystem drivers.
4. **Direct Low-Level Memory & Hardware I/O:**
   - Read/write physical memory and ports using native intrinsics: `mem_write(addr, val)`, `mem_read(addr)`, `outb(port, val)`, `inb(port)`.

---

## Language Keywords

| Keyword | Description |
| :--- | :--- |
| `import` | Includes module/library definitions from `.dk` files into the local program scope. |
| `fnc` / `fn` | Declares a user-defined function block with parameters. |
| `if` | Conditional control block. Conditions are enclosed in parentheses `()`. |
| `while` | Loop control block executing code as long as the condition evaluates to true. |
| `return` | Exits function execution and optionally returns a value to the caller. |
| `mem_write(address, value)` | Writes a raw byte/word directly to physical memory address. |
| `mem_read(address)` | Reads data directly from a physical memory address. |
| `outb(port, value)` | Sends a byte to an x86 hardware I/O port. |
| `inb(port)` | Reads a byte from an x86 hardware I/O port. |

---

## File Types

- **`.k` Files (Direct Instruction Scripts / Main Entry):**
  Contains direct system commands and kernel boot sequences.
- **`.dk` Files (Definition & Library Modules):**
  Contains reusable function, mathematical, and driver declarations (e.g. `lib/math.dk`, `lib/system.dk`).

---

## Code Example

```k
import lib/math.dk
import lib/system.dk

# Direct Boot Statements
screen_write_char(0, 0, 75, 10) # Print 'K' on VGA screen
mouse_init()                     # Initialize PS/2 mouse hardware

fnc main() {
    result = add(100, 200)
    if (result == 300) {
        speaker_emit_tone(440)  # Play beep chime
    }
    return 0
}
```

---

## How to Build the Compiler (`kcc`)

```bash
make
```

## How to Compile a Kernel with `kcc`

```bash
./kcc examples/boot_kernel.k -o kernel.bin
```

To run your bootable kernel in QEMU:
```bash
qemu-system-i386 -kernel kernel.bin
```
