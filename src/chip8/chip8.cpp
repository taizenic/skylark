#include <cstring>
#include <random>
#include <fstream>
#include "emu/chip8.hpp"

static std::random_device device;
static std::mt19937 rng(device());
static std::uniform_int_distribution<u8> dist(0,255);

void chip8::init()
{
    pc = 0x200; //beginning of rom is at 0x200
}

void chip8::load(const char* fname)
{
    std::fstream f;
    f.open(fname, std::ios::binary);

    f.seekg(f.end);
    size_t count = f.tellg();
    f.seekg(f.beg);

    f.read((char*)&ram[0x200], count);

    f.close();
}

void chip8::nextop()
{
    opcode = (ram[pc] << 8) | ram[pc+1]; //get first two bytes straight from ram at pc

    u16 nnn = opcode & 0xfff;
    u16 nn = opcode & 0xff;
    u16 n = opcode & 0xf;
    u16 x = 0xf00 >> 8;
    u16 y = 0xf0 >> 4;

    switch(opcode & 0xf000)
    {
        case 0x0000:
            switch(opcode & 0x0fff)
            {
                case 0x00E0:
                    std::memset(&vram,0,W*H);
                    break;
                case 0x00EE:
                    pc = stack[--sp];
                    break;
            }
            break;
        case 0x1000:
            pc = nnn;
            break;
        case 0x2000:
            stack[sp++] = pc;
            pc = nnn;
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
                    if(v[y] > (0xff - v[x])) v[0xf] = 1;
                    else v[0xf] = 0;
                    v[x] += v[y];
                    break;
                case 0x0005:
                    if(v[y] > (0xff - v[x])) v[0xf] = 0;
                    else v[0xf] = 1;
                    v[x] -= v[y];
                    break;
                case 0x0006:
                    v[0xf] = v[x] & 1;
                    v[x] = v[y] >> 1;
                    break;
                case 0x0007:
                    if(v[y] < (0xff - v[x])) v[0xf] = 0;
                    else v[0xf] = 1;
                    v[x] = v[y] - v[x];
                    break;
                case 0x000E:
                    v[0xf] = (v[x] & 0b10000000) >> 7;
                    v[x] = v[y] = v[y] << 1;
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
            break;
        case 0xC000:
            v[x] = dist(rng) & nn;
            break;
        case 0xD000:
            for(u32 i = 0; i < n; i++)
            {
                for(u32 j = 0; j < 8; j++)
                {
                    if(vram[v[x]][v[y]] == 1 && ram[this->i] & (0b10000000 >> j) != 0) v[0xf] = 1;
                    else v[0xf] = 0;

                    vram[v[x]+j][v[y]+i] ^= (ram[this->i] & (0b10000000 >> j)) >> (7-j);
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
                    for(u32 i = 0; i < 16; i++)
                        if(pad[i])
                        {
                            v[x] = i;
                            return;
                        }
                    break;
                case 0x0015:
                    dt = v[x];
                    break;
                case 0x0018:
                    st = v[x];
                    break;
                case 0x001E:
                    i += v[x];
                    break;
                case 0x0029:
                    i = v[x];
                    break;
                case 0x0033:
                    ram[i] = (v[x] / 100) % 10;
                    ram[i+1] = (v[x] / 10) % 10;
                    ram[i+2] = v[x] % 10;
                    break;
                case 0x0055:
                    for(u32 i = 0; i < x; i++) ram[this->i+i] = v[i];
                    break;
                case 0x0065:
                    for(u32 i = 0; i < x; i++) v[i] = ram[this->i+i];
                    break;
            }
            break;
        default:
            //TODO: make this not ugly
            char* str;
            char** vars;
            std::sprintf(vars[0], "%x", opcode);
            std::sprintf(vars[1], "%x", pc);

            std::strcpy(str, "Could not parse opcode ");
            std::strcat(str, vars[0]);
            std::strcat(str, " at address ");
            std::strcat(str, vars[1]);
            std::strcat(str, ".");

            throw std::runtime_error(str);
            break;
    }
    pc += 2;
}