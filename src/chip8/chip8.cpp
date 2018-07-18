#include <cstring>
#include <random>
#include <fstream>
#include <iostream>
#include "emu/chip8.hpp"

static std::random_device device;
static std::mt19937 rng(device());
static std::uniform_int_distribution<u8> dist(0,255);

void chip8::init()
{
    pc = 0x200; //beginning of rom is at 0x200

    u8 font[16][5] =
    {
        {0xF0, 0x90, 0x90, 0x90, 0xF0}, // 0
        {0x20, 0x60, 0x20, 0x20, 0x70}, // 1
        {0xF0, 0x10, 0xF0, 0x80, 0xF0}, // 2
        {0xF0, 0x10, 0xF0, 0x10, 0xF0}, // 3
        {0x90, 0x90, 0xF0, 0x10, 0x10}, // 4
        {0xF0, 0x80, 0xF0, 0x10, 0xF0}, // 5
        {0xF0, 0x80, 0xF0, 0x90, 0xF0}, // 6
        {0xF0, 0x10, 0x20, 0x40, 0x40}, // 7
        {0xF0, 0x90, 0xF0, 0x90, 0xF0}, // 8
        {0xF0, 0x90, 0xF0, 0x10, 0xF0}, // 9
        {0xF0, 0x90, 0xF0, 0x90, 0x90}, // A
        {0xE0, 0x90, 0xE0, 0x90, 0xE0}, // B
        {0xF0, 0x80, 0x80, 0x80, 0xF0}, // C
        {0xE0, 0x90, 0x90, 0x90, 0xE0}, // D
        {0xF0, 0x80, 0xF0, 0x80, 0xF0}, // E
        {0xF0, 0x80, 0xF0, 0x80, 0x80}  // F
    };
    std::memcpy(&ram, &font, 16*5);
}

void chip8::load(const char* fname)
{
    std::ifstream f;
    f.open(fname, std::ios::binary);

    f.seekg(0, f.end);
    int count = f.tellg();
    f.seekg(0, f.beg);

    f.read((char*)&ram[0x200], count);

    f.close();
}

void chip8::nextop()
{
    opcode = (ram[pc] << 8) | ram[pc+1]; //get first two bytes straight from ram at pc

    u16 nnn = opcode & 0x0fff;
    u16 nn = opcode & 0x00ff;
    u16 n = opcode & 0x000f;
    u16 x = (opcode & 0x0f00) >> 8;
    u16 y = (opcode & 0x00f0) >> 4;

    switch(opcode & 0xf000)
    {
        case 0x0000:
            switch(opcode & 0x00ff)
            {
                case 0x00E0:
                    std::memset(&vram,0u,WIDTH*HEIGHT);
                    break;
                case 0x00EE:
                    pc = stack[--sp];
                    break;
            }
            break;
        case 0x1000:
            pc = nnn;
            pc -= 2;
            break;
        case 0x2000:
            stack[sp++] = pc;
            pc = nnn;
            pc -= 2;
            break;
        case 0x3000:
            if(v[x] == nn) pc += 2;
            break;
        case 0x4000:
            if(v[x] != nn) pc += 2;
            break;
        case 0x5000:
            if(v[x] == v[y]) pc += 2;
            break;
        case 0x6000:
            v[x] = nn;
            break;
        case 0x7000:
            v[x] += nn;
            break;
        case 0x8000:
            switch(opcode & 0x000f)
            {
                case 0x0000:
                    v[x] = v[y];
                    break;
                case 0x0001:
                    v[x] |= v[y];
                    break;
                case 0x0002:
                    v[x] &= v[y];
                    break;
                case 0x0003:
                    v[x] ^= v[y];
                    break;
                case 0x0004:
                    if(v[y] > (0xff - v[x])) v[0xf] = 1u;
                    else v[0xf] = 0u;
                    v[x] += v[y];
                    break;
                case 0x0005:
                    if(v[y] > (0xff - v[x])) v[0xf] = 0u;
                    else v[0xf] = 1u;
                    v[x] -= v[y];
                    break;
                case 0x0006:
                    v[0xf] = v[x] & 1u;
                    v[x] = v[y] >> 1;
                    //v[x] >>= 1;
                    break;
                case 0x0007:
                    if(v[y] < (0xff - v[x])) v[0xf] = 0u;
                    else v[0xf] = 1u;
                    v[x] = v[y] - v[x];
                    break;
                case 0x000E:
                    v[0xf] = (v[x] & 0b10000000) >> 7;
                    v[x] = v[y] << 1;
                    //v[x] <<= 1;
                    break;
            }
            break;
        case 0x9000:
            if(v[x] != v[y]) pc += 2;
            break;
        case 0xA000:
            i = nnn;
            break;
        case 0xB000:
            pc = nnn + v[0];
            pc -= 2;
            break;
        case 0xC000:
            v[x] = dist(rng) & nn;
            break;
        case 0xD000:
            for(u8 i = 0; i < n; i++)
            {
                for(u8 j = 0; j < 8; j++)
                {
                    if((ram[this->i+i] & (0b10000000 >> j)) != 0u)
                    {
                        if(vram[v[y]+i][v[x]+j] != 0u) v[0xf] = 1u;
                        else v[0xf] = 0u;

                        vram[v[y]+i][v[x]+j] ^= 1u;
                    }
                }
            }
            break;
        case 0xE000:
            switch(opcode & 0x00ff)
            {
                case 0x009E:
                    if(pad[v[x]]) pc += 2;
                    break;
                case 0x00A1:
                    if(!pad[v[x]]) pc += 2;
                    break;
            }
            break;
        case 0xF000:
            switch(opcode & 0x00ff)
            {
                case 0x0007:
                    v[x] = dt;
                    break;
                case 0x000A:
                    bool pressed = false;
                    for(u32 i = 0; i < 16; i++)
                    {
                        if(pad[i] != 0u)
                        {
                            v[x] = i;
                            pressed = true;
                        }
                    }
                    if(!pressed) return;

                    break;
                case 0x0015:
                    dt = v[x];
                    break;
                case 0x0018:
                    st = v[x];
                    break;
                case 0x001E:
                    if(i + v[x] > 0xfff) v[0xf] = 1u;
                    else v[0xf] = 0u;
                    i += v[x];
                    break;
                case 0x0029:
                    i = v[x]*0x5;
                    break;
                case 0x0033:
                    ram[i] = (v[x] / 100) % 10;
                    ram[i+1] = (v[x] / 10) % 10;
                    ram[i+2] = v[x] % 10;
                    break;
                case 0x0055:
                    for(u32 i = 0; i < x; i++) ram[this->i+i] = v[i];
                    i += v[x] + 1;
                    break;
                case 0x0065:
                    for(u32 i = 0; i < x; i++) v[i] = ram[this->i+i];
                    i += v[x] + 1;
                    break;
            }
            break;
        default:
            throw std::runtime_error("Unknown Opcode");
            break;
    }
    pc += 2;

    if(dt > 0u) dt--;
    if(st > 0u) st--;
}