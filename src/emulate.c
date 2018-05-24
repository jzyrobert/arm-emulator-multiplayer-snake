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
    wordS largeOffset;
    word binary;
    address Rn;
    address Rd;
    address Operand2;
    address Rs;
    address Rm;
    byte Opcode;
    byte smallOffset;
    bool I;
    bool P;
    bool U;
    bool A;
    bool S;
    bool L;
    enum I_Type type;
} INSTRUCTION;


typedef struct {
    byte mem[MEM_SIZE];
    //memory as 8 bit array
    word reg[REG_SIZE];
    //registers as 32 bit array
    INSTRUCTION instruction;
    word fetch;
    bool instruction_exists;
    bool decode_exists;
    bool finished;
} STATE;


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
    int pc = state->reg[PC];
    state->fetch = (state->mem[pc+3] << 24) + (state->mem[pc + 2] << 16)
            + (state->mem[pc + 1] << 8) + state->mem[pc];
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

void print(STATE *ptr);

//given a word, checks for cond (1 = go, 0 = no)
int checkCond(word instruction, word cpsr) ;

void bitAnd(STATE *state, bool b);

void bitEor(STATE *state, bool b);

void subRO(STATE *state, bool b);

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
    state->instruction.Opcode = (byte) extractBits(b, 21, 24);
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
    //checks if cond is allowed to continue
    if (state->instruction_exists && checkCond(state->instruction.binary, state->reg[CPSR])) {
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
    state->reg[PC] = (uint32_t) ((int32_t) state->reg[PC] + state->instruction.largeOffset);
}

void executeTransfer(STATE *state) {
    printf("test2\n");
}

void executeMult(STATE *state) {
    printf("test3\n");
}


void executeProcess(STATE *state) {
    switch (state->instruction.Opcode) {
        case 0:
            //bit AND
            bitAnd(state, true);
            break;
        case 1:
            //bit EOR
            bitEor(state, true);
            break;
        case 2:
            //Rn-op2
            subRO(state, true);
            break;
        case 3:
            //op2-Rn
            subOR(state);
            break;
        case 4:
            //addition
            break;
        case 8:
            //bit AND without writing
            bitAnd(state, false);
            break;
        case 9:
            //bit EOR without writing
            bitEor(state, false);
            break;
        case 10:
            //Rn-op2 without writing
            subRO(state, false);
            break;
        case 12:
            //bit OR
            break;
        case 13:
            //moving
            break;
        default:
            printf("Invalid OPcode!");
            exit(EXIT_FAILURE);
    }
}

word processOp2(STATE *state) {
    word result;
    if (state->instruction.I) {
        //rotate IMM
        result = extractBits(state->instruction.Operand2, 0, 7);
        int rotateAmount = 2 * extractBits(state->instruction.Operand2, 8, 11);
        result = (result >> rotateAmount) | (result << (32 - rotateAmount));
    } else {
        //shift RM
        result = state->reg[extractBits(state->instruction.Operand2, 0, 3)];
        int shiftAmount = extractBits(state->instruction.Operand2, 7, 11);
        int shiftType = extractBits(state->instruction.Operand2, 5, 6);
        switch (shiftType) {
            case 0:
                if (state->instruction.S) {
                    state->reg[CPSR] ^= ()
                }
                result <<= shiftAmount;
                break;
            case 1:
                result >>= shiftAmount;
                break;
            case 2:
                result = (word) ((int32_t) result >> shiftAmount);
                break;
            case 3:
                result = (result >> 1) | (result << 31);
                break;
            default:
                printf("Invalid shift!");
                exit(EXIT_FAILURE);
        }
    }
    return result;
}

void subRO(STATE *state, bool b) {

}

void bitEor(STATE *state, bool b) {

}

void bitAnd(STATE *state, bool b) {
//b decides if content is written or not
    word op2 = processOp2(state);
    word result = state->reg[state->instruction.Rn] & op2;
    if (b) {
        state->reg[state->instruction.Rd] = result;
    }
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
    STATE *state = calloc(1, sizeof(STATE));
    //initialise(&state);
    //read file
    //1 argument only (so 2 in total)
    if (argc != 2) {
        //error
        printf("Provide only file name as argument\n");
        return EXIT_FAILURE;
    }

    char *file_name = argv[1];
    readFile(file_name, state->mem);

    while (!state->finished) {
    execute(state);

    //prevents pointless execution of below for now
    if (state->finished) {
        break;
    }

    decode(state);

    fetch(state);

    state->reg[PC] += 4;
    }
    //print out stuff
    print(state);


    return EXIT_SUCCESS;
}

void print(STATE *ptr) {
    printf("Registers:\n");
    for (int i = 0; i < 15; ++i) {
        switch (i) {
            case 13:
                printf("%-4s:%9s", "PC", " ");
                printf("%2u (0x%08x)\n", ptr->reg[i+2], ptr->reg[i+2]);
                break;
            case 14:
                printf("%s:%9s", "CPSR", " ");
                printf("%2u (0x%08x)\n", ptr->reg[i+2], ptr->reg[i+2]);
                break;
            default:
                printf("$%-3d:%9s", i, " ");
                printf("%2u (0x%08x)\n", ptr->reg[i], ptr->reg[i]);
        }
    }

    printf("Non-zero memory:\n");
    for (int j = 0; j < MEM_SIZE/4; ++j) {
        for (int i = 0; i < 4; ++i) {
            if (ptr->mem[j*4 + i]) {
                word data = (ptr->mem[j*4] << 24) + (ptr->mem[4*j + 1] << 16) + (ptr->mem[4*j + 2] << 8) + ptr->mem[4*j+3];
                printf("0x%08u: 0x%08x\n",4*j, data);
                break;
            }
        }
    }
}
