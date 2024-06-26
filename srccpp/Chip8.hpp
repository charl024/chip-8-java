#ifndef CHIP8EMULATOR_CHIP8_H
#define CHIP8EMULATOR_CHIP8_H

#include <random>

class Chip8
{
public:
  Chip8();
  void loadROM(char const* file);
  void Cycle();

  unsigned char key[16]{};
  unsigned long graphics[64 * 32]{};

private:
  unsigned short opcode{};
  unsigned char regis[16]{};
  unsigned char memory[4096]{};
  unsigned short index{};
  unsigned short pc;
  unsigned short sp{};

  unsigned char delayTimer{};
  unsigned char soundTimer{};
  unsigned short stack[16]{};

  void loadFont();
  void tableSetup();
  void Table0();
  void Table8();
  void TableE();
  void TableF();

  typedef void (Chip8::*Chip8Func)();
  Chip8Func table[0xF + 1]{};
  Chip8Func table0[0xE + 1]{};
  Chip8Func table8[0xE + 1]{};
  Chip8Func tableE[0xE + 1]{};
  Chip8Func tableF[0x65 + 1]{};


  void OP_NULL();
  void OP_00E0();
  void OP_00EE();
  void OP_1NNN();
  void OP_2NNN();
  void OP_3XNN();
  void OP_4XNN();
  void OP_5XY0();
  void OP_6XNN();
  void OP_7XNN();
  void OP_8XY0();
  void OP_8XY1();
  void OP_8XY2();
  void OP_8XY3();
  void OP_8XY4();
  void OP_8XY5();
  void OP_8XY6();
  void OP_8XY7();
  void OP_8XYE();
  void OP_9XY0();
  void OP_ANNN();
  void OP_BNNN();
  void OP_CXNN();
  void OP_DXYN();
  void OP_EX9E();
  void OP_EXA1();
  void OP_FXO7();
  void OP_FX0A();
  void OP_FX15();
  void OP_FX18();
  void OP_FX1E();
  void OP_FX29();
  void OP_FX33();
  void OP_FX55();
  void OP_FX65();

  std::default_random_engine rand;
  std::uniform_int_distribution<unsigned char> randomByte;
  void randomInit();

};


#endif //CHIP8EMULATOR_CHIP8_H
