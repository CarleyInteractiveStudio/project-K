# K Language Specification (project-K)

`K` is a systems programming and kernel-development language designed for bare-metal software, OS kernels, and driver development with multi-architecture target support, high optimization, and zero external runtime dependencies.

---

## 1. File Types & Structure

- **`.k` files (Direct Instruction / Command Files):**
  - Used for entry points, kernel initialization sequences, and direct hardware instructions.
  - Line-oriented execution without mandatory trailing semicolons `;`.
  - Supports importing definitions from `.dk` files.

- **`.dk` files (Definition & Library Files):**
  - Used for defining functions, data structures, low-level intrinsics, mathematical operations, and hardware drivers (e.g., `matematica.dk`, `vga.dk`, `cpu.dk`).

---

## 2. Syntax & Grammar Principles

1. **Line-Based Statements:**
   - Statements are separated by newlines.
   - Semicolons `;` are optional.

2. **Functions & Control Blocks:**
   - Functions are declared using `fnc` or `fn`.
   - Function signatures specify parameters and optional return types.
   - Code blocks are enclosed in curly braces `{}`.
   - Conditionals and loops use parentheses `()` for conditions.

   ```k
   import matematica.dk

   fnc main() {
       if (x > 0) {
           mem_write(0xB8000, 0x41)
       }
   }
   ```

3. **Direct Memory Operations & Pointers:**
   - Direct memory reading/writing: `mem_write(address, value)` and `mem_read(address)`.
   - Raw pointers and explicit memory layout management without garbage collection or hidden allocations.

---

## 3. Module Import System

`import filename.dk`

Imports symbols and function definitions from `.dk` files, making them available in the local `.k` or `.dk` scope.

---

## 4. Multi-Architecture Target Design

`K` is designed to generate native assembly and bare-metal machine code for multiple architectures:
- `x86_64` (64-bit Intel/AMD)
- `x86` (32-bit Intel)
- `arm64` (AArch64)
- `riscv64` (64-bit RISC-V)

---

## 5. Built-in Build CLI (`kcc`)

The compiler `kcc` is self-contained. `K` projects do not require external build systems like Makefile or CMake to build `.k` / `.dk` source files.

Command usage:
```bash
kcc -o kernel.bin main.k
```
