#include <stdio.h>
#include <stdlib.h>

#include "chip8.h"

chip8 *chip8Init(void)
{

    chip8 *state = malloc(sizeof(chip8));

    state->PC = 0x200;
    state->SP = 0xfa0;
    state->I = 0;
    state->delay = 0;
    state->sound = 0;

    for (int i = 0; i < 16; i++)
    {
        state->V[i] = 0;
    }

    for (int i = 0; i < 4096; i++)
    {
        state->memory[i] = 0;
    }

    return state;
}

// Load rom to memory

void loadROM(chip8 *state, uint8_t *rom, size_t rom_size)
{

    int base = 0x200;

    for (int i = 0; i < rom_size; i++)
    {
        state->memory[base + i] = rom[i];
    }
}

void start_cpu_emu(uint8_t *rom , size_t size )
{

    chip8 *chip8state = chip8Init();
    uint16_t opcode;

    load_initial_values(chip8state);

    loadROM(chip8state, rom, size );

    printf( "%d\n" , size );

    while (chip8state->PC < (0x200 + size))
    {

        opcode = (chip8state->memory[chip8state->PC] << 8) + chip8state->memory[chip8state->PC + 1];

        // Implementing temporary debug
        printf("\nExecute instruction: 0x%x\n", opcode);
        printf("\nCHIP-8 CPU state:\n\n");
        printf("PC\t: 0x%x\n", chip8state->PC);
        printf("SP\t: 0x%x\n", chip8state->SP);
        printf("I\t: 0x%x\n\n", chip8state->I);

        exec_opcode(chip8state, opcode);

        for (int i = 0; i < 0x10; i++)
        {
            printf("V%x\t: 0x%x\n", i, chip8state->V[i]);
        }

        printf("\nSTACK:\n");

        for (int i = 0xf9f, j = 0; j < 10; i -= 2, j++)
        {
            printf("0x%x\t: 0x%x\n", i, chip8state->memory[i]);
        }
    }
}

void exec_opcode(chip8 *state, uint16_t opcode)
{

    int category = (opcode & 0xf000) >> 12;

    switch (category)
    {
    // Device specific special opcodes
    case 0x0:
        switch (opcode)
        {
        case 0x00e0:
            // implement clear screen functionality
            break;

        case 0x00ee:
            state->PC = (state->memory[state->SP++] << 8) + state->memory[state->SP++];
            state->PC += 2;
            break;

        default:
            printf("Unknown opcode : 0x%x\n", opcode);
            state->PC += 2;
            break;
        }
        break;

    // Absolute jump
    case 0x1:
    {
        uint16_t jump_address = opcode & 0xfff;
        state->PC = jump_address;
        break;
    }

    // Jump to subroutine
    case 0x2:
    {
        // PC MSB should be pushed to the stack at SP-1 || implement seperate stack
        uint16_t jump_address = opcode & 0xfff;
        state->memory[state->SP] = state->PC & 0xff;
        state->SP--;
        state->memory[state->SP] = (state->PC & 0xf00) >> 8;
        state->SP--;
        state->PC = jump_address;
        break;
    }

    // Indexed jump
    case 0xb:
    {
        uint16_t jump_address = (opcode & 0xfff) + state->V[0];
        state->PC = jump_address;
        break;
    }

    // Skip if equal to immediate value
    case 0x3:
        if (state->V[(opcode & 0x0f00) >> 8] == (opcode & 0x00ff))
        {
            state->PC += 4;
        }
        else
            state->PC += 2;
        break;

    // Skip if not equal to immediate value
    case 0x4:
        if (state->V[(opcode & 0x0f00) >> 8] != (opcode & 0x00ff))
        {
            state->PC += 4;
        }
        else
            state->PC += 2;
        break;

    // Skip if equal
    case 0x5:
        if (state->V[(opcode & 0x0f00) >> 8] == state->V[(opcode & 0x00f0) >> 4])
        {
            state->PC += 4;
        }
        else
            state->PC += 2;
        break;

    // Skip if not equal
    case 0x9:
        if (state->V[(opcode & 0x0f00) >> 8] != state->V[(opcode & 0x00f0) >> 4])
        {
            state->PC += 4;
        }
        else
            state->PC += 2;
        break;

    // Arithemetic and logic operations
    case 0x8:
    {

        int operation, t1, t2;

        operation =  ( opcode & 0x000f );
        t1 = ( opcode & 0xf00 ) >> 8;
        t2 = ( opcode & 0x0f0 ) >> 4;

        switch ( operation )
        {
        case 0:
            state->V[t1] = state->V[t2];
            break;

        case 1:
            state->V[t1] |= state->V[t2];
            break;

        case 2:
            state->V[t1] &= state->V[t2];
            break;

        case 3:
            state->V[t1] ^= state->V[t2];
            break;

        case 4:
            if (state->V[t1] + state->V[t2] > 0xff)
            {
                state->V[0xf] = 1; // Carry set
            }
            else
                state->V[0xf] = 0;

            state->V[t1] = (state->V[t1] + state->V[t2]) & 0xff;
            break;

        case 5:
            if (state->V[t1] > state->V[t2])
            {
                state->V[0xf] = 1;
            }
            else
                state->V[0xf] = 0;

            state->V[t1] -= state->V[t2];
            break;

        case 6:
            state->V[0xf] = state->V[t1] & 0x1;
            state->V[t1] >>= 1;
            break;

        case 7:
            if (state->V[t2] > state->V[t1])
            {
                state->V[0xf] = 1;
            }
            else
                state->V[0xf] = 0;

            state->V[t1] = state->V[t2] - state->V[t1];
            break;

        case 0xe:
            state->V[0xf] = state->V[t1] & 0x80;
            state->V[t1] <<= 1;
            break;

        default:
            printf("Unknown opcode : 0x%x\n", opcode);
            break;
        }

        state->PC += 2;
        break;
    }

    case 0xa:
        state->I = (opcode & 0xfff);
        state->PC += 2;
        break;

    case 0xc:
    {
        uint8_t rno = 0xff; // implement random number functionality
        state->V[(opcode & 0x0f00) >> 8] = ((opcode & 0xff) & rno);
        state->PC += 2;
        break;
    }

    default:
        printf("opcode needs to be implemented: 0x%x", opcode);
        state->PC += 2;
        break;
    }
}

void load_initial_values(chip8 *state)
{

    for (int i = 0; i < 0x10; i++)
    {
        state->V[i] = i + 10;
    }
}