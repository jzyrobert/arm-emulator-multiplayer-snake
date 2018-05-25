#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "emulate.h"

// MEM_SIZE must be greater than 8 to prevent
#ifndef MEM_SIZE
#define MEM_SIZE 65536
#endif

#ifndef REG_SIZE
#define REG_SIZE 17
#endif

#ifndef PC
#define PC 15
#endif

#ifndef CPSR
#define CPSR 16
#endif

#ifndef PC_OFFSET
#define PC_OFFSET 8
#endif

enum I_Type {PROCESS = 1, MULT, TRANSFER, BRANCH, HALT};

struct instruction {
    wordS largeOffset;
    word binary;
    address Rn;
    address Rd;
    address Operand2;
    address Rs;
    address Rm;
    byte Opcode;
    address smallOffset;
    bool I;
    bool P;
    bool U;
    bool A;
    bool S;
    bool L;
    enum I_Type type;
};

struct state {
    byte mem[MEM_SIZE];
    //memory as 8 bit array
    word reg[REG_SIZE];
    //registers as 32 bit array
    INSTRUCTION instruction;
    word fetch;
    bool instruction_exists;
    bool decode_exists;
    bool finished;
};

void readFile(char* file_name, byte* memory){
    FILE* binary = fopen(file_name, "rb");
    if (!binary) {
        printf("Error occurred, failed to read file!");
        exit(EXIT_FAILURE);
    }
    fread(memory, MEM_SIZE, 1, binary);
    fclose(binary);
}

word fetchData(STATE* state, int start){
    return (state->mem[start + 3] << 24) + (state->mem[start + 2] << 16) +
           (state->mem[start + 1] << 8) + state->mem[start];
}

void writeData(STATE* state, word memLoc, word data){
    state->mem[memLoc] = (byte) data;
    data >>= 8;
    state->mem[memLoc+1] = (byte) data;
    data >>= 8;
    state->mem[memLoc+2] = (byte) data;
    data >>= 8;
    state->mem[memLoc+3] = (byte) data;
}


void fetch(STATE* state) {
    int pc = state->reg[PC];
    state->fetch = fetchData(state, pc);
    state->decode_exists = true;
    state->reg[PC] += 4;
}

word extractBits(word data, int start, int end){
    word r = 0;
    for (int i=start; i<=end; i++)
        r |= 1 << i;

    return (r & data) >> start;
}

enum I_Type getInstruction(word inst);

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
    state->instruction.largeOffset = extractBits(state->instruction.binary, 0, 23) << 2;
    if (state->instruction.largeOffset & (1<<18)) {
        state->instruction.largeOffset = state->instruction.largeOffset | (word) 0xFC000000;
    }
}

void decodeTransfer(STATE *state) {
    word b = state->instruction.binary;
    state->instruction.I = (bool) (b & (1<<25));
    state->instruction.P = (bool) (b & (1<<24));
    state->instruction.U = (bool) (b & (1<<23));
    state->instruction.L = (bool) (b & (1<<20));
    state->instruction.Rn = (address) extractBits(b, 16, 19);
    state->instruction.Rd = (address) extractBits(b, 12, 15);
    state->instruction.smallOffset = (address) extractBits(b, 0, 11);
}

void decodeProcess(STATE *state) {
    word b = state->instruction.binary;
    state->instruction.I = (bool) (b & (1<<25));
    state->instruction.S = (bool) (b & (1<<20));
    state->instruction.Opcode = (byte) extractBits(b, 21, 24);
    state->instruction.Rn = (address) extractBits(b, 16, 19);
    state->instruction.Rd = (address) extractBits(b, 12, 15);
    state->instruction.Operand2 = (address) extractBits(b, 0, 11);
}

void decodeMult(STATE *state) {
    word b = state->instruction.binary;
    state->instruction.A = (bool) (b & (1<<21));
    state->instruction.S = (bool) (b & (1<<20));
    state->instruction.Rd = (address) extractBits(b, 16, 19);
    state->instruction.Rn = (address) extractBits(b, 12, 15);
    state->instruction.Rs = (address) extractBits(b, 8, 11);
    state->instruction.Rm = (address) extractBits(b, 0, 3);
}

void replaceBitDirect(word* destination, int location, int bit) {
    *destination = (*destination & (~(1 << location))) | (bit << location);
}

void replaceBit(word* destination, int location, word source, int location2) {
    int sourceBit = (source & (1<<location2)) >> location2;
    *destination = (*destination & (~(1 << location))) | (sourceBit << location);
}

void updateCPSR(STATE *state, word result) {
    if (state->instruction.S) {
        if (!result) {
            replaceBitDirect(&state->reg[CPSR],30, 1);
        } else {
            replaceBitDirect(&state->reg[CPSR], 30, 0);
        }
        replaceBit(&state->reg[CPSR], 31, result, 31);
    }
}

void execute(STATE* state){
    //checks if cond is allowed to continue
    if (state->instruction_exists && (checkCond(state->instruction.binary
            , state->reg[CPSR]) || !state->instruction.binary)) {
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
    state->reg[PC] = (uint32_t) ((int32_t) state->reg[PC] + (int32_t) state->instruction.largeOffset);
    state->instruction_exists = false;
    state->decode_exists = false;
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
        byte shiftAmount;
        if (state->instruction.Operand2 & (1<<4)) {
            shiftAmount = (byte) state->reg[extractBits(state->instruction.Operand2, 8, 11)];
        } else {
            shiftAmount = (byte) extractBits(state->instruction.Operand2, 7, 11);
        }
        result = state->reg[extractBits(state->instruction.Operand2, 0, 3)];
        int shiftType = extractBits(state->instruction.Operand2, 5, 6);
        switch (shiftType) {
            case 0:
                if (state->instruction.S && (shiftAmount > 0)) {
                    replaceBit(&state->reg[CPSR], 29, result, (32-shiftAmount));
                }
                result <<= shiftAmount;
                break;
            case 1:
                if (state->instruction.S && (shiftAmount > 0)) {
                    replaceBit(&state->reg[CPSR], 29, result, shiftAmount-1);
                }
                result >>= shiftAmount;
                break;
            case 2:
                if (state->instruction.S && (shiftAmount > 0)) {
                    replaceBit(&state->reg[CPSR], 29, result, shiftAmount-1);
                }
                result = (word) ((int32_t) result >> shiftAmount);
                break;
            case 3:
                if (state->instruction.S) {
                    replaceBit(&state->reg[CPSR], 29, result, shiftAmount-1);
                }
                result = (result >> 1) | (result << 31);
                break;
            default:
                printf("Invalid shift!");
                exit(EXIT_FAILURE);
        }
    }
    return result;
}

void executeTransfer(STATE *state) {
    word offSet;
    if (state->instruction.I) {
        state->instruction.I = 0;
        state->instruction.Operand2 = state->instruction.smallOffset;
        offSet = processOp2(state);
    } else {
        offSet = state->instruction.smallOffset;
    }
    word memLoc = state->reg[state->instruction.Rn];
    if(state->instruction.P){
        if(state->instruction.U){
            memLoc += offSet;
        } else {
            memLoc -= offSet;
        }
    } else {
        if(state->instruction.U){
            state->reg[state->instruction.Rn] += offSet;
        } else {
            state->reg[state->instruction.Rn] -= offSet;
        }
    }
    if (memLoc >= (1<<16)) {
        if (memLoc == 0x20200000) {
            printf("One GPIO pin from 0 to 9 has been accessed\n");
        } else if (memLoc == 0x20200004){
            printf("One GPIO pin from 10 to 19 has been accessed\n");
        } else if (memLoc == 0x20200008) {
            printf("One GPIO pin from 20 to 29 has been accessed\n");
        } else if(memLoc == 0x20200028) {
            printf("PIN OFF\n");
        } else if (memLoc == 0x2020001C) {
            printf("PIN ON\n");
        } else {
            printf("Error: Out of bounds memory access at address 0x%08x\n", memLoc);

        }
        if ((memLoc >= 0x20200000) && (memLoc <= 0x2020001c) && state->instruction.L) {
            state->reg[state->instruction.Rd] = memLoc;
        }
    } else {
        if (state->instruction.L) {
            state->reg[state->instruction.Rd] = fetchData(state, (address) memLoc);
        } else {
            writeData(state, (address) memLoc, state->reg[state->instruction.Rd]);
        }
    }
}

void executeMult(STATE *state) {
    state->reg[state->instruction.Rd] = state->reg[state->instruction.Rm]
            * state->reg[state->instruction.Rs];

    if(state->instruction.A){
        state->reg[state->instruction.Rd] += state->reg[state->instruction.Rn];
    }
    if(state->instruction.S){
        updateCPSR(state ,state->reg[state->instruction.Rd]);
    }
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
            addRO(state);
            break;
        case 8:
            //bit addition without writing
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
            bitOR(state);
            break;
        case 13:
            //moving
            processMove(state);
            break;
        default:
            printf("Invalid OPcode!");
            exit(EXIT_FAILURE);
    }
}


void processMove(STATE *state) {
    word op2 = processOp2(state);
    updateCPSR(state, op2);
    state->reg[state->instruction.Rd] = op2;
}

void bitOR(STATE *state) {
    word op2 = processOp2(state);
    word result = state->reg[state->instruction.Rn] | op2;
    updateCPSR(state, result);
    state->reg[state->instruction.Rd] = result;
}

void addRO(STATE *state) {
    word op2 = processOp2(state);
    uint64_t result = state->reg[state->instruction.Rn]+op2;
    if (state->instruction.S) {
        replaceBit(&state->reg[CPSR], 29, (word)(result >> 32), 0);
    }
    updateCPSR(state, (word) result);
    state->reg[state->instruction.Rd] = (word) result;
}

void subOR(STATE *state) {
    word op2 = processOp2(state);
    uint64_t result = (uint64_t) op2 - (uint64_t) state->reg[state->instruction.Rn];
    if (state->instruction.S) {
        replaceBit(&state->reg[CPSR], 29, (word) ~(result >> 32), 0);
    }
    updateCPSR(state, (word) result);
    state->reg[state->instruction.Rd] = (word) result;
}

void subRO(STATE *state, bool b) {
    word op2 = processOp2(state);
    uint64_t result = (uint64_t) state->reg[state->instruction.Rn] - (uint64_t) op2;
    if (state->instruction.S) {
        replaceBit(&state->reg[CPSR], 29,(word) ~(result >> 32), 0);
    }
    updateCPSR(state, (word) result);
    if (b) {
        state->reg[state->instruction.Rd] = (word) result;
    }
}

void bitEor(STATE *state, bool b) {
    word op2 = processOp2(state);
    word result = state->reg[state->instruction.Rn] ^ op2;
    updateCPSR(state, result);
    if (b) {
        state->reg[state->instruction.Rd] = result;
    }
}

void bitAnd(STATE *state, bool b) {
//b decides if content is written or not
    word op2 = processOp2(state);
    word result = state->reg[state->instruction.Rn] & op2;
    updateCPSR(state, result);
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
            return ((instruction & (op << 28)) != 0) == !nEqV;
            //10x1 or 10x0
        }
    } else {
        //0xxx
        return ((instruction & (op << 28)) != 0) == !zSet;
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
                printf("%-4s: ", "PC");
                printf("%10d (0x%08x)\n", ptr->reg[i+2], ptr->reg[i+2]);
                break;
            case 14:
                printf("%s: ", "CPSR");
                printf("%10d (0x%08x)\n", ptr->reg[i+2], ptr->reg[i+2]);
                break;
            default:
                printf("$%-3d: ", i);
                printf("%10d (0x%08x)\n", ptr->reg[i], ptr->reg[i]);
        }
    }

    printf("Non-zero memory:\n");
    for (int j = 0; j < MEM_SIZE/4; ++j) {
        for (int i = 0; i < 4; ++i) {
            if (ptr->mem[j*4 + i]) {
                word data = (ptr->mem[j*4] << 24) + (ptr->mem[4*j + 1] << 16) + (ptr->mem[4*j + 2] << 8) + ptr->mem[4*j+3];
                printf("0x%08x: 0x%08x\n",4*j, data);
                break;
            }
        }
    }
}
