#include <stdio.h>
#include <stdlib.h>

#include "chip8.h"

int main(int argc, char const *argv[]){
    
    uint8_t opcode[] = { 0x83 , 0xA0 , 0x53 , 0xA0 , 0x83 , 0x16 , 0x83 , 0x16 , 0x12 , 0x20 }; //Test opcodes
    size_t rom_size = sizeof( opcode );

    start_cpu_emu( opcode , rom_size );

    return 0;
}
