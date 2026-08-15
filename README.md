# project-K

`project-K` is a standalone programming language and compiler (`kcc`) created specifically for bare-metal systems, operating system kernels, and low-level hardware development across multiple CPU architectures (x86_64, x86, ARM64, RISC-V).

## Documentation
See [SPEC.md](SPEC.md) for full language specifications and syntax definitions.

## Project Structure
- `SPEC.md`: Complete language architecture and syntax specification.
- `src/`: C++ source code for the `kcc` compiler.
- `examples/`: Sample `.k` and `.dk` files demonstrating kernel entry and driver definitions.

## Building the Compiler
```bash
make
```

## Compiling a K Project
```bash
./kcc -o kernel.bin examples/main.k
```
