#include <cstring>
#include <fstream>
#include <iostream>
#include "chip8.h"
#include "mem.h"

Chip8::Chip8()
{
    mem         = new Mem();
    pc          = 0x200;
    I           = 0;
    sp          = 0;
    sound_timer = 0;
    delay_timer = 0;

    memset(V, 0, sizeof(V));
    memset(stack, 0, sizeof(stack));
    memset(display, 0, sizeof(display));
    memset(keypad, 0, sizeof(keypad));
    draw_flag = false;
}
Chip8::~Chip8()
{
    delete mem;
}
bool Chip8::load_rom(string rom_path)
{
    // rom_path = "roms/15PUZZLE";
    // rom_path = "roms/BLINKY";
    // rom_path = "roms/BLITZ";
    // rom_path = "roms/BRIX";
    // rom_path = "roms/CONNECT4";
    // rom_path = "roms/GUESS";
    // rom_path = "roms/HIDDEN";
    // rom_path = "roms/INVADERS";
    // rom_path = "roms/KALEID";
    // rom_path = "roms/MAZE";
    // rom_path = "roms/MERLIN";
    // rom_path = "roms/MISSILE";
    // rom_path = "roms/PONG";
    // rom_path = "roms/PONG2";
    // rom_path = "roms/PUZZLE";
    // rom_path = "roms/SYZYGY";
    // rom_path = "roms/TANK";
    // rom_path = "roms/TETRIS";
    // rom_path = "roms/TICTAC";
    // rom_path = "roms/UFO";
    // rom_path = "roms/VBRIX";
    // rom_path = "roms/VERS";
    // rom_path = "roms/WIPEOFF";
    if (!mem->load_rom(rom_path)) {
        if (!mem->load_rom("roms/BRIX")) {
            return false;
        }
    }
    return true;
}
void Chip8::run(bool trace_mode, bool sound_on)
{
    int opcode = (mem->get(pc) << 8) | (mem->get(pc + 1));

    if (trace_mode) {
        printf("%.4X %.4X %.2X ", pc, opcode, sp);
        for (int i = 0; i < 15; i++) {
            printf("%.2X ", V[i]);
        }
        printf("\n");
    }

    exec(opcode, sound_on);
}
void Chip8::exec(int opcode, bool sound_on)
{
    int val, reg, reg1, reg2;

    switch (get_nibble(opcode, 12, 0xF000)) {
        case 0:
            switch (opcode) {
                case 0x00E0:
                    memset(display, 0, sizeof(display));
                    draw_flag = true;
                    pc += 2;
                    break;
                case 0x00EE:
                    sp--;
                    pc = stack[sp];
                    pc += 2;
                    break;
                default:
                    std::cerr << "Unknown opcode- " << opcode;
                    break;
            }
            break;
        case 1:
            pc = opcode & 0x0FFF;
            break;
        case 2:
            stack[sp] = pc;
            sp++;
            pc = opcode & 0x0FFF;
            break;
        case 3:
            val = get_nibble(opcode, 0, 0x00FF);
            reg = get_nibble(opcode, 8, 0x0F00);
            pc += 2;
            if (V[reg] == val) {
                pc += 2;
            }
            break;
        case 4:
            val = get_nibble(opcode, 0, 0x00FF);
            reg = get_nibble(opcode, 8, 0x0F00);
            pc += 2;
            if (V[reg] != val) {
                pc += 2;
            }
            break;
        case 5:
            reg1 = get_nibble(opcode, 8, 0x0F00);
            reg2 = get_nibble(opcode, 4, 0x00F0);
            pc += 2;
            if (V[reg1] == V[reg2]) {
                pc += 2;
            }
            break;
        case 6:
            val    = get_nibble(opcode, 0, 0x00FF);
            reg    = get_nibble(opcode, 8, 0x0F00);
            V[reg] = val;
            pc += 2;
            break;
        case 7:
            val = get_nibble(opcode, 0, 0x00FF);
            reg = get_nibble(opcode, 8, 0x0F00);
            V[reg] += val;
            pc += 2;
            break;
        case 8:
            val = get_nibble(opcode, 0, 0x000F);
            switch (val) {
                case 0:
                    reg1    = get_nibble(opcode, 8, 0x0F00);
                    reg2    = get_nibble(opcode, 4, 0x00F0);
                    V[reg1] = V[reg2];
                    pc += 2;
                    break;
                case 1:
                    reg1 = get_nibble(opcode, 8, 0x0F00);
                    reg2 = get_nibble(opcode, 4, 0x00F0);
                    V[reg1] |= V[reg2];
                    V[0xF] = 0;
                    pc += 2;
                    break;
                case 2:
                    reg1 = get_nibble(opcode, 8, 0x0F00);
                    reg2 = get_nibble(opcode, 4, 0x00F0);
                    V[reg1] &= V[reg2];
                    V[0xF] = 0;
                    pc += 2;
                    break;
                case 3:
                    reg1 = get_nibble(opcode, 8, 0x0F00);
                    reg2 = get_nibble(opcode, 4, 0x00F0);
                    V[reg1] ^= V[reg2];
                    V[0xF] = 0;
                    pc += 2;
                    break;
                case 4:
                    reg1 = get_nibble(opcode, 8, 0x0F00);
                    reg2 = get_nibble(opcode, 4, 0x00F0);
                    if (V[reg1] + V[reg2] > 0xFF) {
                        V[0xF] = 1;
                    } else {
                        V[0xF] = 0;
                    }
                    V[reg1] += V[reg2];
                    V[reg1] = (int8_t)V[reg1];
                    pc += 2;
                    break;
                case 5:
                    reg1 = get_nibble(opcode, 8, 0x0F00);
                    reg2 = get_nibble(opcode, 4, 0x00F0);
                    if (V[reg1] < V[reg2]) {
                        V[0xF] = 0;
                    } else {
                        V[0xF] = 1;
                    }
                    V[reg1] = (uint8_t)V[reg1] - (uint8_t)V[reg2];
                    pc += 2;
                    break;
                case 6:
                    reg    = get_nibble(opcode, 8, 0x0F00);
                    V[0xF] = V[reg] & 0x1;
                    V[reg] >>= 1;
                    V[reg] = (uint8_t)V[reg];
                    pc += 2;
                    break;
                case 7:
                    reg1 = get_nibble(opcode, 8, 0x0F00);
                    reg2 = get_nibble(opcode, 4, 0x00F0);
                    if (V[reg1] > V[reg2]) {
                        V[0xF] = 0;
                    } else {
                        V[0xF] = 1;
                    }
                    V[reg1] = (uint8_t)V[reg2] - (uint8_t)V[reg1];
                    pc += 2;
                    break;
                case 0xE:
                    reg    = get_nibble(opcode, 8, 0x0F00);
                    V[0xF] = V[reg] >> 7;
                    V[0xF] = (uint8_t)V[0xF];
                    V[reg] <<= 1;
                    V[reg] = (uint8_t)V[reg];
                    pc += 2;
                    break;
                default:
                    std::cerr << "Unknown opcode- " << opcode;
                    break;
            }
            break;
        case 9:
            reg1 = get_nibble(opcode, 8, 0x0F00);
            reg2 = get_nibble(opcode, 4, 0x00F0);
            pc += 2;
            if (V[reg1] != V[reg2]) {
                pc += 2;
            }
            break;
        case 10:
            I = opcode & 0x0FFF;
            pc += 2;
            break;
        case 11:
            pc = (opcode & 0x0FFF);
            pc += V[0];
            break;
        case 12:
            val    = get_nibble(opcode, 0, 0x00FF);
            reg    = get_nibble(opcode, 8, 0x0F00);
            V[reg] = 5 & val;
            pc += 2;
            break;
        case 13: {
            reg1  = get_nibble(opcode, 8, 0x0F00);
            reg2  = get_nibble(opcode, 4, 0x00F0);
            int x = V[reg1], y = V[reg2];
            int ht  = opcode & 0x000F;
            int wt  = 8;
            V[0x0F] = 0;
            for (int i = 0; i < ht; i++) {
                int pixel = mem->get(I + i);
                for (int j = 0; j < wt; j++) {
                    if ((pixel & (0x80 >> j)) != 0) {
                        int index = ((x + j) + ((y + i) * 64)) % 2048;
                        if (display[index] == 1) {
                            V[0x0F] = 1;
                        }
                        display[index] ^= 1;
                    }
                }
            }
            draw_flag = true;
            pc += 2;
            break;
        }
        case 14:
            val = get_nibble(opcode, 0, 0x00FF);
            switch (val) {
                case 0x9E:
                    reg = get_nibble(opcode, 8, 0x0F00);
                    pc += 2;
                    if (keypad[V[reg]] != 0) {
                        pc += 2;
                    }
                    break;
                case 0xA1:
                    reg = get_nibble(opcode, 8, 0x0F00);
                    pc += 2;
                    if (keypad[V[reg]] == 0) {
                        pc += 2;
                    }
                    break;
                default:
                    std::cerr << "Invalid opcode-> " << std::hex << opcode;
                    break;
            }
            break;
        case 15:
            val = get_nibble(opcode, 0, 0x00FF);
            switch (val) {
                case 0x07:
                    reg    = get_nibble(opcode, 8, 0x0F00);
                    V[reg] = delay_timer;
                    pc += 2;
                    break;
                case 0x0A: {
                    bool key_pressed = false;
                    reg              = get_nibble(opcode, 8, 0x0F00);
                    for (int i = 0; i < 16; i++) {
                        if (keypad[i] != 0) {
                            key_pressed = true;
                            V[reg]      = (uint8_t)i;
                        }
                    }
                    if (key_pressed) {
                        pc += 2;
                    }
                    break;
                }
                case 0x15:
                    reg         = get_nibble(opcode, 8, 0x0F00);
                    delay_timer = V[reg];
                    pc += 2;
                    break;
                case 0x18:
                    reg         = get_nibble(opcode, 8, 0x0F00);
                    sound_timer = V[reg];
                    pc += 2;
                    break;
                case 0x1E:
                    reg = get_nibble(opcode, 8, 0x0F00);
                    if (I + V[reg] > 0xFFF) {
                        V[0xF] = 1;
                    } else {
                        V[0xF] = 0;
                    }
                    I += V[reg];
                    I = (uint16_t)I;
                    pc += 2;
                    break;
                case 0x29:
                    reg = get_nibble(opcode, 8, 0x0F00);
                    I   = V[reg] * 0x5;
                    pc += 2;
                    break;
                case 0x33:
                    reg = get_nibble(opcode, 8, 0x0F00);
                    mem->set(I, (uint8_t)((uint8_t)V[reg] / 100));
                    mem->set(I + 1, (uint8_t)((uint8_t)(V[reg] / 10) % 10));
                    mem->set(I + 2, (uint8_t)((uint8_t)(V[reg] % 100) % 10));
                    pc += 2;
                    break;
                case 0x55:
                    reg = get_nibble(opcode, 8, 0x0F00);
                    for (int i = 0; i <= reg; i++) {
                        mem->set(I + i, V[i]);
                    }
                    I = I + reg + 1;
                    I = (uint16_t)I;
                    pc += 2;
                    break;
                case 0x65:
                    reg = get_nibble(opcode, 8, 0x0F00);
                    for (int i = 0; i <= reg; i++) {
                        V[i] = mem->get(I + i);
                    }
                    I = I + reg + 1;
                    I = (uint16_t)I;
                    pc += 2;
                    break;
                default:
                    std::cerr << "Invalid opcode -> " << std::hex << opcode << std::endl;
                    break;
            }
            break;
        default:
            std::cerr << "Invalid opcode -> " << std::hex << opcode << std::endl;
            break;
    }
    if (delay_timer > 0) {
        delay_timer--;
    }
    if (sound_timer > 0) {
        if (sound_on) {
            printf("\a\n");
        }
        sound_timer--;
    }
}
int Chip8::get_nibble(int val, int bits, int bval = 0xFFFF)
{
    return ((val & bval) >> bits);
}
bool Chip8::get_draw_flag()
{
    return draw_flag;
}
void Chip8::set_draw_flag(bool flag)
{
    draw_flag = flag;
}
int Chip8::get_display_value(int i)
{
    return display[i];
}
void Chip8::set_keypad_value(int index, int val)
{
    keypad[index] = val;
}
