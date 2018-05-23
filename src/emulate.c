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
}

int main(int argc, char **argv) {
    STATE new;
    initialise(&new);

    //read file
    //1 argument only (so 2 in total)
    if (argc != 2) {
        //error
        printf("Provide file name as argument\n");
        return EXIT_FAILURE;
    }


    //load into mem
    //fetch
    //decode
    //execute

  return EXIT_SUCCESS;
}
