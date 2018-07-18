#pragma once
#include "util/types.hpp"
#include "util/constants.hpp"

struct chip8
{
    u8 ram[4096] {0u}; //4k of memory
    u8 vram[HEIGHT][WIDTH] {0u}; //graphics ram. resolution is 64x32
    u8 v[16] {0u}; //16 8-bit registers
    u16 i {0u}; //a 16-bit register
    u16 pc {0u}; //program counter
    u16 opcode {0u}; //current opcode at mem[pc]
    u16 stack[MAX_STACK] {0u}; //stack with 16 levels
    u16 sp {0u}; //stack pointer. points to top level of stack
    u8 pad[16] {0u}; //input
    u8 dt {0u}; //delay timer
    u8 st {0u}; //sound timer

    bool drawflag {false};

    void init(); //initializes interpreter
    void load(const char* fname); //loads file into memory
    void nextop(); //executes next opcode
};