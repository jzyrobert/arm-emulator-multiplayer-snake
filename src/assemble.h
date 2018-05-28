#include "typedefs.h"

#ifndef PROCESSOR_FLOW_TYPEDEFS
#define PROCESSOR_FLOW_TYPEDEFS
typedef struct assembly ASSEMBLY;
typedef struct label LABEL;
typedef struct state STATE;
typedef struct NTF nameToFunc;
#endif

typedef word (*evalFunc)(ASSEMBLY *assembly, STATE *state);

//helpers

bool isHEX(char* string);

bool isNUM(char* string);

bool isDirectLDR(char* string);

void setAlwaysCond(word *output);

void setBits(word *output, word bits, word end);

void setBranchOffset(STATE *state, ASSEMBLY *as, word *output);

void processTransfers(ASSEMBLY *as, STATE *state, word *output);

void stripBrackets(char* string);

void evalShifts (ASSEMBLY *as, word *output, int op2Point);

void RemoveSpaces(char* source);

void evalOperand2(ASSEMBLY *as, word *output, int op2Point);

void evalExp (ASSEMBLY *as, word *output, int op2Point);

word getRegNum(char *name);

word calculateBOffset(STATE *state, ASSEMBLY *as, char *offset);

long decodeEXP(char *str);

int isEven(word num);

int lowestBit(word imm);

int highestBit(word imm);

int rangeOfBits(word imm);


//Data processing

word evalAdd(ASSEMBLY *as, STATE *state);

word evalSub(ASSEMBLY *as, STATE *state);

word evalMov(ASSEMBLY *as, STATE *state);

word evalRsb(ASSEMBLY *as, STATE *state);

word evalAnd(ASSEMBLY *as, STATE *state);

word evalEor(ASSEMBLY *as, STATE *state);

word evalOrr(ASSEMBLY *as, STATE *state);

word evalTst(ASSEMBLY *as, STATE *state);

word evalTeq(ASSEMBLY *as, STATE *state);

word evalCmp(ASSEMBLY *as, STATE *state);

//Multiplication

word evalMul(ASSEMBLY *as, STATE *state);

word evalMla(ASSEMBLY *as, STATE *state);

//Branching

word evalBranc(ASSEMBLY *as, STATE *state);

word evalBNE(ASSEMBLY *as, STATE *state);

word evalBLT(ASSEMBLY *as, STATE *state);

word evalBGE(ASSEMBLY *as, STATE *state);

word evalBGT(ASSEMBLY *as, STATE *state);

word evalBLE(ASSEMBLY *as, STATE *state);

word evalBeq(ASSEMBLY *as, STATE *state);

//Data transfer

word evalLDR(ASSEMBLY *as, STATE *state);

word evalSTR(ASSEMBLY *as, STATE *state);

//Special

word evalLSL(ASSEMBLY *as, STATE *state);

word evalANDEQ(ASSEMBLY *as, STATE *state);

//Data processing

evalFunc functionLookup(char *lookUp);

void assignLabels(STATE *state);

void writeToFile(STATE *state, word binary);

void pass2(STATE *state);