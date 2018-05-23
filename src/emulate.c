#include <stdlib.h>
#include <stdint.h>

#define MEM_SIZE 65536
#define REG_SIZE 17

typedef struct {
    uint8_t mem[MEM_SIZE];
    //memory as 8 bit array
    uint32_t reg[REG_SIZE];
    //registers as 32 bit array
    uint32_t PC;
} STATE;

void initialise(STATE* state) {
    //sets everything to 0
    state->PC = 0;
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
    }


    //load into mem
    //decode
    //execute

  return EXIT_SUCCESS;
}
