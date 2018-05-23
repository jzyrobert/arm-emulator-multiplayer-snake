#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

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


enum I_Type {PROCESS = 1, MULT, TRANSFER, BRANCH};

typedef struct {
    enum I_Type I;
    word binary;
    bool I;
    bool P;
    bool U;
    bool A;
    bool S;
    bool S;
    address Rn;
    address Rd;
    address Operand2;
    address Rs;
    address Rm;
    byte smallOffset;
    word largeOffset;
} INSTRUCTION;

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

void readFile(char* file_name, byte* memory){
    FILE* binary = fopen(file_name, "rb");
    if (!binary) {
        printf("Error occurred, failed to read file!");
        exit(EXIT_FAILURE);
    }
    fread(memory, MEM_SIZE, 1, binary);
    fclose(binary);
}

enum I_type getInstruction(word inst) {
    word op = 1;
    if ((inst & op<<27) == 1) {
        //1x
        return BRANCH;
    } else{
        if ((inst & op<<26) == 1) {
            //01x
            return TRANSFER;
        } else {
            //00x
            if (((inst & op<<7)== 1) && ((inst & op<<6)== 0) && ((inst & op<<5)== 0) && ((inst & op<<4)== 1)) {
                //1001 at bits 7-4
                return MULT;
            } else{
                return PROCESS;
            }
        }
    }
}

int main(int argc, char **argv) {
    STATE new;
    initialise(&new);

    //read file
    //1 argument only (so 2 in total)
    if (argc != 2) {
        //error
        printf("Provide only file name as argument\n");
        return EXIT_FAILURE;
    }

    char* file_name = argv[1];
    readFile(file_name, new.mem);

    for (int i = 0; i < MEM_SIZE; ++i) {
        if (new.mem[i] != 0) {
            printf("%d \n", new.mem[i]);
        }
    }

    //fetch
    //decode
    //execute

  return EXIT_SUCCESS;
}
