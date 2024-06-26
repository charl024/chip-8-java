#include "Chip8.hpp"
#include <fstream>
#include <chrono>
#include <random>

const unsigned int FONT_SET_SIZE = 80;
const unsigned int FONT_SET_ADDRESS = 0x50;
const unsigned int START_ADDRESS = 0x200;

unsigned char fontSet[FONT_SET_SIZE] =
  {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
  };

Chip8::Chip8()
{
  randomInit();
  pc = START_ADDRESS;
  loadFont();
  tableSetup();

}

void Chip8::loadROM(const char *fileRom)
{
  std::ifstream file(fileRom, std::ios::binary | std::ios::ate);
  if (file.is_open())
  {
    std::streampos size = file.tellg();
    char* buff = new char[size];

    file.seekg(0, std::ios::beg);
    file.read(buff, size);
    file.close();

    for (long i = 0; i < size; ++i)
    {
      memory[START_ADDRESS + i] = buff[i];
    }

    delete[] buff;
  }
}

void Chip8::randomInit()
{
  rand.seed(std::chrono::system_clock::now().time_since_epoch().count());
  randomByte = std::uniform_int_distribution<unsigned char>(0, 255);
}

void Chip8::Cycle()
{
  opcode = (memory[pc] << 8) | memory[pc + 1];
  pc += 2;

  (this->*table[(opcode & 0xF000) >> 12])();

  if (delayTimer > 0) --delayTimer;
  if (soundTimer > 0) --soundTimer;
}

void Chip8::tableSetup()
{
  table[0x0] = &Chip8::Table0;
  table[0x1] = &Chip8::OP_1NNN;
  table[0x2] = &Chip8::OP_2NNN;
  table[0x3] = &Chip8::OP_3XNN;
  table[0x4] = &Chip8::OP_4XNN;
  table[0x5] = &Chip8::OP_5XY0;
  table[0x6] = &Chip8::OP_6XNN;
  table[0x7] = &Chip8::OP_7XNN;

  table[0x8] = &Chip8::Table8;
  table[0x9] = &Chip8::OP_9XY0;
  table[0xA] = &Chip8::OP_ANNN;
  table[0xB] = &Chip8::OP_BNNN;
  table[0xC] = &Chip8::OP_CXNN;
  table[0xD] = &Chip8::OP_DXYN;

  table[0xE] = &Chip8::TableE;
  table[0xF] = &Chip8::TableF;

  for (size_t i = 0; i <= 0xE; i++)
  {
    table0[i] = &Chip8::OP_NULL;
    table8[i] = &Chip8::OP_NULL;
    tableE[i] = &Chip8::OP_NULL;
  }

  table0[0x0] = &Chip8::OP_00E0;
  table0[0xE] = &Chip8::OP_00EE;

  table8[0x0] = &Chip8::OP_8XY0;
  table8[0x1] = &Chip8::OP_8XY1;
  table8[0x2] = &Chip8::OP_8XY2;
  table8[0x3] = &Chip8::OP_8XY3;
  table8[0x4] = &Chip8::OP_8XY4;
  table8[0x5] = &Chip8::OP_8XY5;
  table8[0x6] = &Chip8::OP_8XY6;
  table8[0x7] = &Chip8::OP_8XY7;
  table8[0xE] = &Chip8::OP_8XYE;

  tableE[0x1] = &Chip8::OP_EXA1;
  tableE[0xE] = &Chip8::OP_EX9E;

  for (size_t i = 0; i <= 0x65; i++)
  {
    tableF[i] = &Chip8::OP_NULL;
  }

  tableF[0x07] = &Chip8::OP_FXO7;
  tableF[0x0A] = &Chip8::OP_FX0A;
  tableF[0x15] = &Chip8::OP_FX15;
  tableF[0x18] = &Chip8::OP_FX18;
  tableF[0x1E] = &Chip8::OP_FX1E;
  tableF[0x29] = &Chip8::OP_FX29;
  tableF[0x33] = &Chip8::OP_FX33;
  tableF[0x55] = &Chip8::OP_FX55;
  tableF[0x65] = &Chip8::OP_FX65;

}

void Chip8::Table0()
{
  (this->*(table0[opcode & 0x000Fu]))();
}

void Chip8::Table8()
{
  (this->*(table8[opcode & 0x000Fu]))();
}

void Chip8::TableE()
{
  (this->*(tableE[opcode & 0x000Fu]))();
}

void Chip8::TableF()
{
  (this->*(tableF[opcode & 0x00FFu]))();
}


void Chip8::loadFont()
{
  for (unsigned i = 0; i < FONT_SET_SIZE; i++) {
    memory[FONT_SET_ADDRESS + i] = fontSet[i];
  }
}

void Chip8::OP_NULL()
{
  // does nothing
}

void Chip8::OP_00E0()
{
  for(unsigned long & graphic : graphics) graphic = 0x0;
}

void Chip8::OP_00EE()
{
  --sp;
  pc = stack[sp];
}

void Chip8::OP_1NNN()
{
  pc = opcode & 0xFFF;
}

void Chip8::OP_2NNN()
{
  stack[sp] = pc;
  ++sp;
  pc = opcode & 0xFFF;
}

void Chip8::OP_3XNN()
{
  unsigned char vx = (opcode & 0x0F00) >> 8;
  unsigned char byte = opcode & 0x00FF;

  if (regis[vx] == byte) pc += 2;
}

void Chip8::OP_4XNN()
{
  unsigned char vx = (opcode & 0x0F00) >> 8;
  unsigned char byte = opcode & 0x00FF;

  if (regis[vx] != byte) pc += 2;
}

void Chip8::OP_5XY0()
{
  unsigned char vx = (opcode & 0x0F00) >> 8;
  unsigned char vy = (opcode & 0x00F0) >> 4;

  if (regis[vx] == regis[vy]) pc += 2;
}

void Chip8::OP_6XNN()
{
  unsigned char vx = (opcode & 0x0F00) >> 8;
  unsigned char byte = opcode & 0x00FF;

  regis[vx] = byte;
}

void Chip8::OP_7XNN()
{
  unsigned char vx = (opcode & 0x0F00) >> 8;
  unsigned char byte = opcode & 0x00FF;

  regis[vx] += byte;
}

void Chip8::OP_8XY0()
{
  unsigned char vx = (opcode & 0x0F00) >> 8;
  unsigned char vy = (opcode & 0x00F0) >> 4;

  regis[vx] = regis[vy];
}

void Chip8::OP_8XY1()
{
  unsigned char vx = (opcode & 0x0F00) >> 8;
  unsigned char vy = (opcode & 0x00F0) >> 4;

  regis[vx] |= regis[vy];
}

void Chip8::OP_8XY2()
{
  unsigned char vx = (opcode & 0x0F00) >> 8;
  unsigned char vy = (opcode & 0x00F0) >> 4;

  regis[vx] &= regis[vy];
}

void Chip8::OP_8XY3()
{
  unsigned char vx = (opcode & 0x0F00) >> 8;
  unsigned char vy = (opcode & 0x00F0) >> 4;

  regis[vx] ^= regis[vy];
}

void Chip8::OP_8XY4()
{
  unsigned char vx = (opcode & 0x0F00) >> 8;
  unsigned char vy = (opcode & 0x00F0) >> 4;

  unsigned int sum = regis[vx] + regis[vy];

  if (sum > 255) regis[0xF] = 1;
  else regis[0xF] = 0;

  regis[vx] = sum & 0xFF;
}

void Chip8::OP_8XY5()
{
  unsigned char vx = (opcode & 0x0F00) >> 8;
  unsigned char vy = (opcode & 0x00F0) >> 4;

  if (regis[vx] > regis[vy]) regis[0xF] = 1;
  else regis[0xF] = 0;

  regis[vx] -= regis[vy];
}

void Chip8::OP_8XY6()
{
  unsigned char vx = (opcode & 0x0F00) >> 8;

  regis[0xF] = (regis[vx] & 0x1);
  regis[vx] >>= 1;
}

void Chip8::OP_8XY7()
{
  unsigned char vx = (opcode & 0x0F00) >> 8;
  unsigned char vy = (opcode & 0x00F0) >> 4;

  if (regis[vy] > regis[vx]) regis[0xF] = 1;
  else regis[0xF] = 0;

  regis[vx] = regis[vy] - regis[vx];
}

void Chip8::OP_8XYE()
{
  unsigned char vx = (opcode & 0x0F00) >> 8;

  regis[0xF] = (regis[vx] & 0x80) >> 7;
  regis[vx] <<= 1;
}

void Chip8::OP_9XY0()
{
  unsigned char vx = (opcode & 0x0F00) >> 8;
  unsigned char vy = (opcode & 0x00F0) >> 4;

  if (regis[vx] != regis[vy]) pc += 2;
}

void Chip8::OP_ANNN()
{
  index = opcode & 0x0FFF;
}

void Chip8::OP_BNNN()
{
  pc = regis[0] + (opcode & 0x0FFF);
}

void Chip8::OP_CXNN()
{
  unsigned char vx = (opcode & 0x0F00) >> 8;
  unsigned char byte = opcode & 0x00FF;

  regis[vx] = randomByte(rand) & byte;
}

void Chip8::OP_DXYN()
{
  unsigned char vx = (opcode & 0x0F00) >> 8;
  unsigned char vy = (opcode & 0x00F0) >> 4;
  unsigned char height = opcode & 0x000F;

  unsigned char x = regis[vx] % 64;
  unsigned char y = regis[vy] % 32;

  regis[0xF] = 0;

  for (unsigned int row = 0; row < height; ++row)
  {
    unsigned char sprite = memory[index + row];
    for (unsigned int col = 0; col < 8; ++col)
    {
      unsigned char pixel = sprite & (0x80 >> col);
      unsigned long* screenPixel = &graphics[(y + row) * 64 + (x + col)];

      if (pixel)
      {
        if (*screenPixel == 0xFFFFFFFF)
        {
          regis[0xF] = 1;
        }
        *screenPixel ^= 0xFFFFFFFF;
      }
    }
  }
}

void Chip8::OP_EX9E()
{
  unsigned char vx = (opcode & 0x0F00) >> 8;
  unsigned char keyVal = regis[vx];

  if (key[keyVal]) pc += 2;
}

void Chip8::OP_EXA1()
{
  unsigned char vx = (opcode & 0x0F00) >> 8;
  unsigned char keyVal = regis[vx];

  if (!key[keyVal]) pc += 2;
}

void Chip8::OP_FXO7()
{
  unsigned char vx = (opcode & 0x0F00) >> 8;
  regis[vx] = delayTimer;
}

void Chip8::OP_FX0A()
{
  unsigned char vx = (opcode & 0x0F00) >> 8;
  if (key[0]) regis[vx] = 0;
  else if (key[1]) regis[vx] = 1;
  else if (key[2]) regis[vx] = 2;
  else if (key[3]) regis[vx] = 3;
  else if (key[4]) regis[vx] = 4;
  else if (key[5]) regis[vx] = 5;
  else if (key[6]) regis[vx] = 6;
  else if (key[7]) regis[vx] = 7;
  else if (key[8]) regis[vx] = 8;
  else if (key[9]) regis[vx] = 9;
  else if (key[10]) regis[vx] = 10;
  else if (key[11]) regis[vx] = 11;
  else if (key[12]) regis[vx] = 12;
  else if (key[13]) regis[vx] = 13;
  else if (key[14]) regis[vx] = 14;
  else if (key[15]) regis[vx] = 15;
  else pc -= 2;
}

void Chip8::OP_FX15()
{
  unsigned char vx = (opcode & 0x0F00) >> 8;
  delayTimer = regis[vx];
}

void Chip8::OP_FX18()
{
  unsigned char vx = (opcode & 0x0F00) >> 8;
  soundTimer = regis[vx];
}

void Chip8::OP_FX1E()
{
  unsigned char vx = (opcode & 0x0F00) >> 8;
  index += regis[vx];
}

void Chip8::OP_FX29()
{
  unsigned char vx = (opcode & 0x0F00) >> 8;
  index = FONT_SET_ADDRESS + (5 * regis[vx]);
}

void Chip8::OP_FX33()
{
  unsigned char vx = (opcode & 0x0F00) >> 8;
  unsigned char val = regis[vx];

  memory[index + 2] = val % 10;
  val /= 10;
  memory[index + 1] = val % 10;
  val /= 10;
  memory[index] = val % 10;
}

void Chip8::OP_FX55()
{
  unsigned char vx = (opcode & 0x0F00) >> 8;
  for (unsigned char i = 0; i <= vx; ++i) {
    memory[index + i] = regis[i];
  }
}

void Chip8::OP_FX65()
{
  unsigned char vx = (opcode & 0x0F00) >> 8;
  for (unsigned char i = 0; i <= vx; ++i) {
    regis[i] = memory[index + i];
  }
}


