#ifndef CHIP8_CHIP8EMULATOR_H
#define CHIP8_CHIP8EMULATOR_H
#include <cstdint>
#include <string>

class Mem;
class Chip8 {
  public:
    Mem *mem = nullptr;

  private:
    uint8_t  V[16];
    uint16_t I, pc;
    uint8_t  sp;

    uint16_t stack[16];
    int      display[64 * 32];
    int      keypad[16];

    uint8_t delay_timer, sound_timer;
    bool    draw_flag;
    int     get_nibble(int, int, int);

  public:
    Chip8();
    ~Chip8();

    bool load_rom(std::string rom_path);

    bool get_draw_flag();
    void set_draw_flag(bool);

    void run(bool trace_mode, bool sound_on);
    void exec(int, bool);
    int  get_display_value(int);
    void set_keypad_value(int, int);
};
#endif