# MCC (Modular C Compiler)

A strict, self-hosting C11 compiler targeting x86-64 Linux.

## Overview
MCC is written in strict Object-Oriented C. It creates a 1:1 translation of C source code to x86-64 assembly without optimizations.

## Build
Run the immutable makefile:
```bash
make
```
## Usage
```bash
./mcc input.c -o output
```

## Documentation
See ```docs/``` for directory structure, coding standards, and naming conventions.