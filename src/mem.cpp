#include "mem.h"
#include <fstream>

void Mem::init()
{
    reset();
}
uint8_t Mem::get(uint16_t addr)
{
    return ram[addr];
}
void Mem::set(uint16_t addr, uint8_t data)
{
    ram[addr] = data;
}
bool Mem::load_rom(std::string rom_path)
{
    std::ifstream f(rom_path, std::ios::binary | std::ios::in);
    if (!f.is_open()) {
        return false;
    }
    char c;
    int  j = 512;
    for (int i = 0x200; f.get(c); i++) {
        if (j >= 4096)
            return false;
        ram[i] = (uint8_t)c;
        j++;
    }
    return true;
}
void Mem::set_fontset()
{
    for (int i = 0; i < 80; i++) {
        set(i, chip8_fontset[i]);
    }
}
void Mem::reset()
{
    for (size_t i = 0; i < 4096; i++) {
        ram[i] = 0;
    }
    set_fontset();
}