#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#define MEM_SIZE 65536
#define REG_SIZE 17
#define PC 15
#define CPSR 16
#define PC_OFFSET 8

typedef uint8_t byte;
typedef uint32_t word;
typedef int32_t wordS;
typedef uint16_t address;

enum I_Type {PROCESS = 1, MULT, TRANSFER, BRANCH, HALT};

typedef struct {
    enum I_Type type;
    word binary;
    bool I;
    bool P;
    bool U;
    bool A;
    bool S;
    bool L;
    address Rn;
    address Rd;
    address Operand2;
    address Rs;
    address Rm;
    byte smallOffset;
    wordS largeOffset;
} INSTRUCTION;


typedef struct {
    byte mem[MEM_SIZE];
    //memory as 8 bit array
    word reg[REG_SIZE];
    //registers as 32 bit array
    word pc;
    word fetch;
    word decode;
    INSTRUCTION instruction;
    bool instruction_exists;
    bool decode_exists;
    bool finished;
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
    INSTRUCTION I;
    state->instruction = I;
    state->finished = false;
    state->instruction_exists = false;
    state->decode_exists = false;
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

void fetch(STATE* state) {
    int pc = state->pc;
    state->fetch = (state->mem[pc+3] << 24) + (state->mem[pc + 2] << 16) + (state->mem[pc + 1] << 8) + state->mem[pc];
    state->pc += 4;
    state->decode_exists = true;
}

word extractBits(word data, int start, int end){
    word r = 0;
    for (int i=start; i<=end; i++)
        r |= 1 << i;

    return (r & data) >> start;
}

enum I_Type getInstruction(word inst);


void decodeMult(STATE *state);

void decodeProcess(STATE *state);

void decodeTransfer(STATE *state);

void decodeBranch(STATE *state);

void executeProcess(STATE *state);

void executeMult(STATE *state);

void executeTransfer(STATE *state);

void executeBranch(STATE *state);

void decode(STATE* state) {
    if (state->decode_exists) {
        state->instruction.binary = state->fetch;
        state->instruction.type = getInstruction(state->fetch);
        state->instruction_exists = true;
        switch ((int) state->instruction.type) {
            case 1 :
                decodeProcess(state);
                break;
                case 2:
                    decodeMult(state);
                    break;
                case 3:
                    decodeTransfer(state);
                    break;
                    case 4:
            decodeBranch(state);
            break;
            case 5:
                break;
        default:
            printf("Invalid state!\n");
            exit(EXIT_FAILURE);
            //error
    }
}
}

void decodeBranch(STATE *state) {
    wordS offSet = extractBits(state->instruction.binary, 0, 23) << 2;
    state->instruction.largeOffset = offSet;
}

void decodeTransfer(STATE *state) {
    word b = state->instruction.binary;
    state->instruction.I = (b & (1<<25)) ? true : false;
    state->instruction.P = (b & (1<<24)) ? true : false;
    state->instruction.U = (b & (1<<23)) ? true : false;
    state->instruction.L = (b & (1<<20)) ? true : false;
    state->instruction.Rn = (address) extractBits(b, 16, 19);
    state->instruction.Rd = (address) extractBits(b, 12, 15);
    state->instruction.smallOffset = (byte) extractBits(b, 0, 11);
}

void decodeProcess(STATE *state) {
    word b = state->instruction.binary;
    state->instruction.I = (b & (1<<25)) ? true : false;
    state->instruction.S = (b & (1<<20)) ? true : false;
    state->instruction.Rn = (address) extractBits(b, 16, 19);
    state->instruction.Rd = (address) extractBits(b, 12, 15);
    state->instruction.Operand2 = (address) extractBits(b, 0, 11);
}

void decodeMult(STATE *state) {
    word b = state->instruction.binary;
    state->instruction.A = (b & (1<<21)) ? true : false;
    state->instruction.S = (b & (1<<20)) ? true : false;
    state->instruction.Rd = (address) extractBits(b, 16, 19);
    state->instruction.Rn = (address) extractBits(b, 12, 15);
    state->instruction.Rs = (address) extractBits(b, 8, 11);
    state->instruction.Rm = (address) extractBits(b, 0, 3);
}

void execute(STATE* state){
    if (state->instruction_exists) {
        switch ((int) state->instruction.type) {
            case 1:
                executeProcess(state);
                break;
            case 2:
                executeMult(state);
                break;
            case 3:
                executeTransfer(state);
                break;
            case 4:
                executeBranch(state);
                break;
            case 5:
                state->finished = true;
                break;
            default:
                printf("Invalid instruction!");
                exit(EXIT_FAILURE);
        }
    }
}

void executeBranch(STATE *state) {
    printf("test1");
}

void executeTransfer(STATE *state) {
    printf("test2");
}

void executeMult(STATE *state) {
    printf("test3");
}

void executeProcess(STATE *state) {
    printf("test4");
}

//quick int to binary
int converted(int i) {
    if (i == 0) {
        return 0;
    } else {
        return (i % 2 + 10 * converted(i / 2));
    }
}

//given a word, checks for cond (1 = go, 0 = no)
int checkCond(word instruction, word cpsr) {
    word op = 1;
    int zSet = cpsr & 1 << 30;
    int nEqV = ((cpsr & 1 << 31) == (cpsr & 1 << 28));
    if (instruction & (op << 31)) {
        //1xxx
        if (instruction & (op << 30)) {
            //11xx
            if (instruction & (op << 28)) {
                //11x1
                return zSet || !nEqV;
            } else {
                //11x0
                if (instruction & (op << 29)) {
                    //1110
                    return 1;
                } else {
                    //1100
                    return !zSet && nEqV;
                }
            }
        } else {
            return instruction & (op << 28) ? !nEqV : nEqV;
            //10x1 or 10x0
        }
    } else {
        //0xxx
        if (instruction & (op << 28)) {
            //0xx1
            //checks if Z is not set
            return !zSet;
        } else {
            //0xx0
            //checks if Z is set
            return zSet;
        }
    }
}


enum I_Type getInstruction(word inst) {
    word op = 1;
    if (!inst) {
        return HALT;
    }
    if (inst & op << 27) {
        //1x
        return BRANCH;
        } else {
        if (inst & op << 26) {
            //01x
            return TRANSFER;
            } else {
            //00x
            if (!(inst & op << 25) && (inst & op << 7) && (inst & op << 4)) {
                //1001 at bits 7-4
                return MULT;
                } else {
                return PROCESS;
                }
        }
    }
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

    char *file_name = argv[1];
    readFile(file_name, state.mem);

    while (!state.finished) {
    execute(&state);

    decode(&state);

    fetch(&state);

    state.pc += 4;
    }
    //print out stuff
    printf("Registers:\n");
    for (int i = 0; i < 13; ++i) {
        printf("$%d  :        ", i);
        printf("%u (0x%x)\n", state.reg[i], state.reg[i]);
    }

    printf("Non-zero memory:\n");
    for (int j = 0; j < MEM_SIZE/4; ++j) {
        for (int i = 0; i < 4; ++i) {
            if (state.mem[j*4 + i]) {
                word data = (state.mem[j*4] << 24) + (state.mem[4*j + 1] << 16) + (state.mem[4*j + 2] << 8) + state.mem[4*j+3];
                printf("0x%x: 0x%x\n",4*j, data);
                break;
            }
        }
    }

    return EXIT_SUCCESS;
}
