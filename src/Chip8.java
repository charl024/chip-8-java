import java.util.Random;

public class Chip8 {
    public char[] key = new char[16];
    public int[] graphics = new int[(Values.WIDTH.getValue() * Values.HEIGHT.getValue())];

    private int opcode;
    private int[] register = new int[16];
    private int[] memory = new int[4096];
    private int index;
    private int pc;
    private int sp;

    private int delay;
    private int sound;
    private int[] stack = new int[16];

    int[] fontSet = {
            0xF0, 0x90, 0x90, 0x90, 0xF0,
            0x20, 0x60, 0x20, 0x20, 0x70,
            0xF0, 0x10, 0xF0, 0x80, 0xF0,
            0xF0, 0x10, 0xF0, 0x10, 0xF0,
            0x90, 0x90, 0xF0, 0x10, 0x10,
            0xF0, 0x80, 0xF0, 0x10, 0xF0,
            0xF0, 0x80, 0xF0, 0x90, 0xF0,
            0xF0, 0x10, 0x20, 0x40, 0x40,
            0xF0, 0x90, 0xF0, 0x90, 0xF0,
            0xF0, 0x90, 0xF0, 0x10, 0xF0,
            0xF0, 0x90, 0xF0, 0x90, 0x90,
            0xE0, 0x90, 0xE0, 0x90, 0xE0,
            0xF0, 0x80, 0x80, 0x80, 0xF0,
            0xE0, 0x90, 0x90, 0x90, 0xE0,
            0xF0, 0x80, 0xF0, 0x80, 0xF0,
            0xF0, 0x80, 0xF0, 0x80, 0x80
    };

    public Chip8() {
        pc = (short) Values.START_ADDRESS.getValue();
        loadFont();

    }

    public void loadRom() {

    }

    public void cycle() {
        opcode = (memory[pc] << 8) | memory[pc + 1];
        pc += 2;

        if (delay > 0) --delay;
        if (sound > 0) --sound;
    }

    private void loadFont() {
        System.arraycopy(fontSet, 0, memory, Values.FONT_ADDRESS.getValue(), fontSet.length);
    }

    private void getOpcode() {

    }

    private void OP_NULL() {}

    // clears graphics
    private void OP_00E0() {
        for(int i: graphics) i = 0x0;
    }

    // returns from subroutine
    private void OP_00EE() {
        --sp;
        pc = stack[sp];
    }

    // jumps to NNN
    private void OP_1NNN() {
        pc = opcode & 0xFFF;
    }

    // calls subroutine at NNN
    private void OP_2NNN() {
        stack[sp] = pc;
        ++sp;
        pc = opcode & 0xFFF;
    }

    // skips next instruction if VX is equal to NN
    private void OP_3XNN() {
        char vx = (char) ((opcode & 0x0F00) >>> 8);
        char nn = (char) (opcode & 0x00FF);

        if (register[vx] == nn) pc += 2;
    }

    // skips next instruction if VX is not equal to NN
    private void OP_4XNN() {
        char vx = (char) ((opcode & 0x0F00) >>> 8);
        char nn = (char) (opcode & 0x00FF);

        if (register[vx] != nn) pc += 2;
    }

    // skips next instruction if VX is equal to VY
    private void OP_5XY0() {
        char vx = (char) ((opcode & 0x0F00) >>> 8);
        char vy = (char) ((opcode & 0x00F0) >>> 4);

        if (register[vx] == register[vy]) pc += 2;
    }

    // sets VX to NN
    private void OP_6XNN() {
        char vx = (char) ((opcode & 0x0F00) >>> 8);
        char nn = (char) (opcode & 0x00FF);

        register[vx] = nn;
    }

    // adds NN to VX
    private void OP_7XNN() {
        char vx = (char) ((opcode & 0x0F00) >>> 8);
        char nn = (char) (opcode & 0x00FF);

        register[vx] += nn;
    }

    // sets VX to VY
    private void OP_8XY0() {
        char vx = (char) ((opcode & 0x0F00) >>> 8);
        char vy = (char) ((opcode & 0x00F0) >>> 4);

        register[vx] = register[vy];
    }

    // ORs VX with VY and sets it to VX
    private void OP_8XY1() {
        char vx = (char) ((opcode & 0x0F00) >>> 8);
        char vy = (char) ((opcode & 0x00F0) >>> 4);

        register[vx] |= register[vy];
    }

    // ANDs VX with VY and sets it to VX
    private void OP_8XY2() {
        char vx = (char) ((opcode & 0x0F00) >>> 8);
        char vy = (char) ((opcode & 0x00F0) >>> 4);

        register[vx] &= register[vy];
    }

    // XORs VX with VY and sets it to VX
    private void OP_8XY3() {
        char vx = (char) ((opcode & 0x0F00) >>> 8);
        char vy = (char) ((opcode & 0x00F0) >>> 4);

        register[vx] ^= register[vy];
    }

    // adds VX to VY, sets VF to 1 if there is a carry
    private void OP_8XY4() {
        char vx = (char) ((opcode & 0x0F00) >>> 8);
        char vy = (char) ((opcode & 0x00F0) >>> 4);

        char sum = (char) (register[vx] + register[vy]);

        if (sum > 255) register[0xF] = 1;
        else register[0xF] = 0;

        register[vx] = sum & 0xFF;
    }

    // subtracts VY from VX, sets VF to 1 is VX > VY otherwise set to 0
    private void OP_8XY5() {
        char vx = (char) ((opcode & 0x0F00) >>> 8);
        char vy = (char) ((opcode & 0x00F0) >>> 4);

        if (register[vx] >= register[vy]) register[0xF] = 1;
        else register[0xF] = 0;

        char diff = (char) (register[vx] - register[vy]);

        register[vx] = diff;
    }

    // stores LSB of VX in VF, then right shifts VX by 1
    private void OP_8XY6() {
        char vx = (char) ((opcode & 0x0F00) >>> 8);

        register[0xF] = register[vx] & 1;
        register[vx] >>>= 1;
    }

    // sets VX to VY - VX, sets VF to 0 if VY > VX otherwise set to 0
    private void OP_8XY7() {
        char vx = (char) ((opcode & 0x0F00) >>> 8);
        char vy = (char) ((opcode & 0x00F0) >>> 4);

        if (register[vy] > register[vx]) register[0xF] = 1;
        else register[0xF] = 0;

        char diff = (char) (register[vy] - register[vx]);

        register[vx] = diff;
    }

    // stores MSB of VX in VF, then left shifts VX by 1
    private void OP_8XYE() {
        char vx = (char) ((opcode & 0x0F00) >>> 8);

        register[0xF] = (register[vx] & 0x80) >>> 7;
        register[vx] <<= 1;
    }

    // skips next instruction if VX is not equal to VY
    private void OP_9XY0() {
        char vx = (char) ((opcode & 0x0F00) >>> 8);
        char vy = (char) ((opcode & 0x00F0) >>> 4);

        if (register[vx] != register[vy]) pc += 2;
    }

    // sets index to NNN
    private void OP_ANNN() {
        index = opcode & 0x0FFF;
    }

    // jump to NNN plus V0
    private void OP_BNNN() {
        pc = register[0] + (opcode & 0x0FFF);
    }

    // sets VX to the result of a bitwise AND on a random num
    private void OP_CXNN() {
        char vx = (char) ((opcode & 0x0F00) >>> 8);
        char nn = (char) (opcode & 0x00FF);
        Random random = new Random(System.currentTimeMillis());
        register[vx] = random.nextInt(255) & nn;
    }

    // handles pixels and drawing
    private void OP_DXYN() {
        char vx = (char) ((opcode & 0x0F00) >>> 8);
        char vy = (char) ((opcode & 0x00F0) >>> 4);
        char height = (char) (opcode & 0x000F);

        char x = (char) (register[vx] % 64);
        char y = (char) (register[vy] % 32);

        register[0xF] = 0;

        for (int i = 0; i < height; i++) {
            char sprite = (char) memory[index + i];
            for (int j = 0; j < 8; j++) {
                char pixel = (char) (sprite & (0x80 >>> j));
                int screen = graphics[(y + i) * 64 + (x + j)];

                if (pixel > 0) {
                    if (screen == 0xFFFFFFFF) register[0xF] = 1;
                    screen ^= 0xFFFFFFFF;
                }
            }
        }
    }

    private void OP_EX9E() {
        char vx = (char) ((opcode & 0x0F00) >>> 8);
        char keyVal = (char) register[vx];
        if (key[keyVal] > 0) pc += 2;
    }

    private void OP_EXA1() {
        char vx = (char) ((opcode & 0x0F00) >>> 8);
        char keyVal = (char) register[vx];
        if (key[keyVal] == 0) pc += 2;
    }

    private void OP_FX07() {
        char vx = (char) ((opcode & 0x0F00) >>> 8);
        register[vx] = delay;
    }

    private void OP_FX0A() {
        char vx = (char) ((opcode & 0x0F00) >>> 8);
        if (key[0] > 0) register[vx] = 0;
        else if (key[1] > 0) register[vx] = 1;
        else if (key[2] > 0) register[vx] = 2;
        else if (key[3] > 0) register[vx] = 3;
        else if (key[4] > 0) register[vx] = 4;
        else if (key[5] > 0) register[vx] = 5;
        else if (key[6] > 0) register[vx] = 6;
        else if (key[7] > 0) register[vx] = 7;
        else if (key[8] > 0) register[vx] = 8;
        else if (key[9] > 0) register[vx] = 9;
        else if (key[10] > 0) register[vx] = 10;
        else if (key[11] > 0) register[vx] = 11;
        else if (key[12] > 0) register[vx] = 12;
        else if (key[13] > 0) register[vx] = 13;
        else if (key[14] > 0) register[vx] = 14;
        else if (key[15] > 0) register[vx] = 15;
        else pc -= 2;
    }

    private void OP_FX15() {
        char vx = (char) ((opcode & 0x0F00) >>> 8);
        delay = register[vx];
    }

    private void OP_FX18() {
        char vx = (char) ((opcode & 0x0F00) >>> 8);
        sound = register[vx];
    }

    private void OP_FX1E() {
        char vx = (char) ((opcode & 0x0F00) >>> 8);
        index += register[vx];
    }

    private void OP_FX29() {
        char vx = (char) ((opcode & 0x0F00) >>> 8);
        index = Values.FONT_ADDRESS.getValue() + (5 * register[vx]);
    }

    private void OP_FX33() {
        char vx = (char) ((opcode & 0x0F00) >>> 8);
        char value = (char) register[vx];

        memory[index + 2] = value % 10;
        value /= 10;
        memory[index + 1] = value % 10;
        value /= 10;
        memory[index] = value % 10;
    }

    private void OP_FX55() {
        char vx = (char) ((opcode & 0x0F00) >>> 8);
        System.arraycopy(register, 0, memory, index, vx + 1);
    }

    private void OP_FX65() {
        char vx = (char) ((opcode & 0x0F00) >>> 8);
        System.arraycopy(memory, index, register, 0, vx + 1);
    }








}
