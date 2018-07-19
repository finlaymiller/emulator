# Emulator for the X-Makina Project

This program emulates Dr. Larry Hughes' X-Makina ISA. From his website:

X-Makina is a 16-bit Instruction Set Architecture (ISA) design with the following features:

* 33 instructions (memory access, arithmetic, logic, and control), all 16-bits in width. Many instructions can operate on a byte, a word, or both.
* An additional 32 instructions that can be emulated by using existing instructions, allowing operations such as subroutine return, interrupt return, and stack push and pull.
* A common instruction format, containing an opcode and one or two operand addresses (for arithmetic and logical instructions) or a signed 10-bit offset (for jump instructions).
* Eight 16-bit registers: four special purpose (program counter, stack pointer, program status word, and link register) and four general purpose (for data, addressing, or both).
* Four addressing modes (register, register with pre- and post- auto-increment and auto-decrement, register-relative, and immediate).
* A 16-bit data/address bus, allowing up to 64 KiB of random-access memory.

The ISA exhibits many features found in existing commercial load-store or RISC (reduced instruction set computer) machines.

It supports breakpoints, input and output devices, interrupts, and signal handling.