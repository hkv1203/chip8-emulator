#include<stdint.h>

typedef struct chip8
{
    //Initialize registers
    uint8_t V[16];
    uint16_t PC; 
    uint16_t SP;
    uint16_t I;

    uint16_t delay;
    uint8_t sound;

    uint8_t memory[4096];

}chip8;

//Initialize chip8 memory states
chip8* chip8Init(void);

//Load the ROM data
void loadROM( chip8* , uint8_t* , size_t );
void start_cpu_emu( uint8_t* , size_t );
void exec_opcode( chip8* , uint16_t );

//Initialize random test values for the CPU states
void load_initial_values( chip8* );