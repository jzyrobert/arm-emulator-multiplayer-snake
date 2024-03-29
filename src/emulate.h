#include <stdint.h>
#include <stdbool.h>
#include "typedefs.h"

#ifndef PROCESSOR_FLOW_TYPEDEFS
#define PROCESSOR_FLOW_TYPEDEFS
typedef struct instruction INSTRUCTION;
typedef struct state STATE;

// Decode bits into the state struct depending on the instruction to decode
void decodeMult(STATE *state);

void decodeProcess(STATE *state);

void decodeTransfer(STATE *state);

void decodeBranch(STATE *state);

// Executes the necessary instruction
void executeProcess(STATE *state);

void executeMult(STATE *state);

void executeTransfer(STATE *state);

void executeBranch(STATE *state);

// Processes a two operand instruction
word processOp2(STATE *state);

// Prints the state of the registers and memory
void print(STATE *ptr);

// Given a word, checks for cond (1 = go, 0 = no)
int checkCond(word instruction, word cpsr) ;

// Sets the state according to the necessary instruction, depending on the
// value of b
void bitAnd(STATE *state, bool b);

void bitEor(STATE *state, bool b);

void subRO(STATE *state, bool b);

void subOR(STATE *state);

void addRO(STATE *state);

void bitOR(STATE *state);

void processMove(STATE *state);

#endif

