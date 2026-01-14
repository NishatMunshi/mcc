# MCC (Modular C Compiler)

A strict, self-hosting C23 compiler targeting x86-64 Linux.

## Goal
1. Write the bare minimum to compile its own source code.
2. Support x86-64 linux fully (eventually, give me a break lol)

## Overview
MCC is written in a modular fashion.
## Build
Run the immutable makefile:
```bash
make
```
## Usage
```bash
./mcc input.c
```