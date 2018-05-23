#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#define MEM_SIZE 65536
#define REG_SIZE 17
#define PC 15
#define CPSR 16

typedef uint8_t byte;
typedef uint32_t word;
typedef uint16_t address;

typedef struct {
    byte mem[MEM_SIZE];
    //memory as 8 bit array
    word reg[REG_SIZE];
    //registers as 32 bit array
    word pc;
    word fetch;
    word decode;
} STATE;

void initialise(STATE* state) {
    //sets everything to 0
    state->pc = 0;
    for (int i = 0; i < REG_SIZE; ++i) {
        state->reg[i] = 0;
    }
    for (int j = 0; j < MEM_SIZE; ++j) {
        state->mem[j] = 0;
    }
    state->fetch = 0;
    state->decode = 0;
}

void readFile(char* file_name, byte* memory){
    FILE* binary = fopen(file_name, "rb");
    if (!binary) {
        printf("Error occurred, failed to read file!");
        exit(EXIT_FAILURE);
    }
    fread(memory, MEM_SIZE, 1, binary);
    fclose(binary);
}

void fetch(STATE* state){
    int pc = state->pc;
    state->fetch = (state->mem[pc] << 24) + (state->mem[pc+1] << 16) + (state->mem[pc+2] << 8) + state->mem[pc+3];
    state->pc += 4;
}

int main(int argc, char **argv) {
    STATE state;
    initialise(&state);

    //read file
    //1 argument only (so 2 in total)
    if (argc != 2) {
        //error
        printf("Provide only file name as argument\n");
        return EXIT_FAILURE;
    }

    char* file_name = argv[1];
    readFile(file_name, state.mem);

    for (int i = 0; i < MEM_SIZE; ++i) {
        if (state.mem[i] != 0) {
            printf("0x%02x \n", state.mem[i]);
        }
    }


    fetch(&state);
    //decode
    //execute

  return EXIT_SUCCESS;
}
