#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "assemble.h"
#include "emulate.c"

struct label {
    char label[20];
    word address;
} ;

struct state{
    LABEL labels[10];
    word extras[10];
    int noOfLines;
    int noOfLabels;
    int noOfExtraLines;
    char* input;
    FILE* outputFile;
};

struct assembly{
    char* tokens[6];
    word address;
    int noOfTokens;
};

struct NTF{
    char *name;
    evalFunc func;
};

const nameToFunc funcMap[] = {{"add", evalAdd}, {"sub", evalSub},
                              {"rsb", evalRsb}, {"and", evalAnd},
                              {"eor", evalEor}, {"orr", evalOrr},
                              {"mov", evalMov}, {"tst", evalTst},
                              {"teq", evalTeq}, {"cmp", evalCmp},
                              {"mul", evalMul}, {"mla", evalMla},
                              {"b", evalBranc}, {"beq", evalBeq},
                              {"add" ,evalAdd}, {"sub" ,evalSub},
                              {"ldr", evalLDR}, {"str", evalSTR},
                              {"bne", evalBNE}, {"bge", evalBGE},
                              {"blt", evalBLT}, {"bgt", evalBGT},
                              {"ble", evalBLE}, {"lsl", evalLSL},
                              {"andeq", evalANDEQ}};

bool isHEX(char* string) {
    return strchr(string, 'x') != NULL;
}

bool isNUM(char* string) {
    return strchr(string, '#') != NULL;
}

bool isDirectLDR(char* string) {
    return strchr(string, '=') != NULL;
}

void setAlwaysCond(word *output){
    setBits(output, 0xE, flagV);
}

void setBits(word *output, word bits, word end){
    //the bits are set leading to the end bit
    //end bit is the lowest bit of the section being set
    *output |= (bits << end);
}

void setBranchOffset(STATE *state, ASSEMBLY *as, word *output) {
    word offset = calculateBOffset(state, as,as->tokens[0]);
    offset &= 0xFFFFFF;
    setBits(output, offset, 0);
}

void stripBrackets(char* string) {
    size_t len = strlen(string);
    memmove(string, string+1, len-2);
    string[len-2] = 0;
}

void processTransfers(ASSEMBLY *as, STATE *state, word *output) {
    printf("Processing index assembly:\n");
    if (strchr(as->tokens[1], ']') != NULL) {
        //[RN]- something?
        if (as->noOfTokens == 2) {
            //pre-index RN
            stripBrackets(as->tokens[1]);
            setBits(output, getRegNum(as->tokens[1]), 16);
            setBits(output, 1, ppIndexBit);
            setBits(output, 1, upBit);
        } else {
            //post index [RN], expression
            stripBrackets(as->tokens[1]);
            if (strchr(as->tokens[2], '-') == NULL) {
                //its not negative, sets U;
                setBits(output, 1, upBit);
            }
            //set Rn
            setBits(output, getRegNum(as->tokens[1]), 16);
            if (as->noOfTokens == 3) {
                //not shift
                if (isNUM(as->tokens[2])) {
                    //sets immediate value
                    setBits(output, (word) decodeEXP(as->tokens[2]), 0);
                } else {
                    //sets register
                    setBits(output, 1, immediateOffset);
                    setBits(output, (word) getRegNum(as->tokens[2]), 0);
                }
            } else {
                //shift case
                setBits(output, 1, immediateOffset);
                if (strchr(as->tokens[2], '-') != NULL) {
                    //if negative remove sign before processing;
                    as->tokens[2] = as->tokens[2] + 1;
                }
                evalShifts(as, output, 2);
            }
        }
    } else {
        //[Rn - Expression]
        setBits(output, 1, ppIndexBit);
        //Sets P
        if (strchr(as->tokens[2], '-') == NULL) {
            //its not negative, sets U;
            setBits(output, 1, upBit);
        }
        //Setting RN
        setBits(output,getRegNum(as->tokens[1] + 1) , 16);
        if (as->noOfTokens == 3) {
            if (strchr(as->tokens[2], 'r') != NULL) {
                //reg
                setBits(output, 1, immediateOffset);
                setBits(output, (word) getRegNum(as->tokens[2]), 0);
            } else {
                //Imm value
                setBits(output, (word) decodeEXP(as->tokens[2]), 0);
            }
        } else {
            //optional shift case
            setBits(output, 1, immediateOffset);
            if (strchr(as->tokens[2], '-') != NULL) {
                //if negative remove sign before processing;
                as->tokens[2] = as->tokens[2] + 1;
            }
            //removes ]
            as->tokens[3][strlen(as->tokens[3])-2] = '\n';
            as->tokens[3][strlen(as->tokens[3])-1] = '\0';
            evalShifts(as, output, 2);
        }
    }
}

void evalOperand2(ASSEMBLY *as, word *output, int op2Point){

    if (isNUM(as->tokens[op2Point]) || isHEX(as->tokens[op2Point])){
        evalExp(as, output, op2Point);
    } else {
        evalShifts(as, output, op2Point);
    }
}

void evalExp (ASSEMBLY *as, word *output, int op2Point){
    setBits(output, 1, 25);
    word imm = (word) decodeEXP(as->tokens[op2Point]);
    if (imm < 256) {
        //no shift needed
        setBits(output, imm, 0);
    } else if (imm > ((255 << 24) + 1)) {
        printf("Number too big!");
        exit(EXIT_FAILURE);
    } else {
        int n = rangeOfBits(imm);
        if (n > 7) {
            printf("Number can't be represented!");
            exit(EXIT_FAILURE);
        }
        if ((n == 7) && !isEven((word) lowestBit(imm))) {
            printf("Odd shift cannot be represented!");
        }
        word rotate = 0;
        while ((imm > 255) || !isEven((word) rotate)) {
            //rotate left 1
            imm = (imm << 1) | (imm >> 31);
            rotate++;
        }
        rotate = rotate / 2;
        setBits(output, imm, 0);
        setBits(output, rotate, 8);
    }
}

void RemoveSpaces(char* source) {
    char* i = source;
    char* j = source;
    while(*j != 0)
    {
        *i = *j++;
        if(*i != ' ')
            i++;
    }
    *i = 0;
}

void evalShifts (ASSEMBLY *as, word *output, int op2Point) {
    if ((as->noOfTokens - op2Point) < 2) {
        //no shift
        as->tokens[op2Point][strlen(as->tokens[op2Point]) - 1] = '\0';
        setBits(output, getRegNum(as->tokens[op2Point]), 0);
    } else {
        //shift
        setBits(output, (word) getRegNum(as->tokens[op2Point]), 0);
        if (strchr(as->tokens[op2Point + 1], 'l') != NULL) {
            //lsl or lsr
            if (as->tokens[op2Point + 1][2] == 'r') {
                //lsr
                setBits(output, 1, 5);
            }
        } else if (strchr(as->tokens[op2Point + 1], 'a') != NULL) {
            //asr
            setBits(output, 2, 5);
        } else {
            //ror
            setBits(output, 3, 5);
        }
        //process Rs or OP
        if (isNUM(as->tokens[op2Point + 1])) {
            setBits(output, (word) decodeEXP(as->tokens[op2Point + 1]+3), 7);
        } else {
            setBits(output, 1, 4);
            setBits(output, getRegNum(as->tokens[op2Point + 1] + 3), 8);
        }
    }
}

word getRegNum(char *name){
    name++;
    if (strchr(name, 'C')) {
        return 15;
    }
    return (word) strtol(name, NULL, 10);
}

word calculateBOffset(STATE *state, ASSEMBLY *as, char *offset) {
    word address = 0;
    if (strchr(offset, '\n') != NULL) {
        offset[strlen(offset)-1] = '\0';
    }
    for (int i = 0; i < state->noOfLabels; ++i) {
        if (strcmp(offset, state->labels[i].label) == 0) {
            address = state->labels[i].address;
            break;
        }
    }
    address = (word) (((int32_t) address - (int32_t) as->address - 8) >> 2 & ((1 << 25) - 1));
    return address;
}

long decodeEXP(char *str) {
    int offset = 0;
    int base = 10;
    if (strchr(str, '-') != NULL) {
        //removes -
        offset++;
    }
    if (isDirectLDR(str)) {
        //removes =
        offset++;
    }
    if (isHEX(str)) {
        //removes the 0x
        offset += 2;
        base = 16;
    }
    if (isNUM(str)){
        offset ++;
        //removes #
    }
    return strtol(str + offset, NULL, base);
}

int isEven(word num) {
    return !(num % 2);
}

int lowestBit(word imm) {
    int lowest = 0;
    for (int i = 0; i < 32; ++i) {
        if (imm & (1 << i)) {
            lowest = i;
            break;
        }
    }
    return lowest;
}

int highestBit(word imm) {
    int highest=0;
    for (int j = 31; j >=0 ; --j) {
        if (imm & (1 << j)) {
            highest = j;
            break;
        }
    }
    return highest;
}

int rangeOfBits(word imm) {
    return highestBit(imm) - lowestBit(imm);
}

word evalAdd(ASSEMBLY *as, STATE *state) {
    word output = 0;
    setAlwaysCond(&output);
    setBits(&output, 4, ABit);
    evalOperand2(as, &output, 2);
    setBits(&output, getRegNum(as->tokens[0]), 12);
    setBits(&output, getRegNum(as->tokens[1]), 16);
    return output;
}

word evalSub(ASSEMBLY *as, STATE *state) {
    word output = 0;
    setAlwaysCond(&output);
    setBits(&output, 2, ABit);
    setBits(&output, getRegNum(as->tokens[0]), 12);
    setBits(&output, getRegNum(as->tokens[1]), 16);
    evalOperand2(as, &output, 2);
    return output;
}

word evalMov(ASSEMBLY *as, STATE *state){
    word output = 0;
    setAlwaysCond(&output);
    setBits(&output, 13, ABit);
    setBits(&output, getRegNum(as->tokens[0]), 12);
    evalOperand2(as, &output, 1);
    return output;
}

word evalRsb(ASSEMBLY *as, STATE *state){
    word output = 0;
    setAlwaysCond(&output);
    setBits(&output, getRegNum(as->tokens[0]), 12);
    setBits(&output, getRegNum(as->tokens[1]), 16);
    setBits(&output, 3, ABit);
    evalOperand2(as, &output, 2);
    return output;
}

word evalAnd(ASSEMBLY *as, STATE *state){
    word output = 0;
    setAlwaysCond(&output);
    setBits(&output, getRegNum(as->tokens[0]), 12);
    setBits(&output, getRegNum(as->tokens[1]), 16);
    evalOperand2(as, &output, 2);
    return output;
}

word evalEor(ASSEMBLY *as, STATE *state){
    word output = 0;
    setAlwaysCond(&output);
    setBits(&output, getRegNum(as->tokens[0]), 12);
    setBits(&output, getRegNum(as->tokens[1]), 16);
    setBits(&output, 1, ABit);
    evalOperand2(as, &output, 2);;
    return output;
}

word evalOrr(ASSEMBLY *as, STATE *state){
    word output = 0;
    setAlwaysCond(&output);
    setBits(&output, getRegNum(as->tokens[0]), 12);
    setBits(&output, getRegNum(as->tokens[1]), 16);
    setBits(&output, 12, ABit);
    evalOperand2(as, &output, 2);
    return output;
}

word evalTst(ASSEMBLY *as, STATE *state){
    word output = 0;
    setAlwaysCond(&output);
    setBits(&output, 1, SBit);
    setBits(&output, 8, ABit);
    setBits(&output, getRegNum(as->tokens[0]), 16);
    evalOperand2(as, &output, 1);
    return output;
}

word evalTeq(ASSEMBLY *as, STATE *state){
    word output = 0;
    setAlwaysCond(&output);
    setBits(&output, 1, SBit);
    setBits(&output, 9, ABit);
    setBits(&output, getRegNum(as->tokens[0]), 16);
    evalOperand2(as, &output, 1);
    return output;
}

word evalCmp(ASSEMBLY *as, STATE *state){
    word output = 0;
    setAlwaysCond(&output);
    setBits(&output, 1, SBit);
    setBits(&output, 10, ABit);
    setBits(&output, getRegNum(as->tokens[0]), 16);
    evalOperand2(as, &output, 1);
    return output;
}

word evalMul(ASSEMBLY *as, STATE *state){
    word output = 0;
    setAlwaysCond(&output);
    setBits(&output, getRegNum(as->tokens[0]), 16); //sets Rd
    setBits(&output, getRegNum(as->tokens[1]), 0); //sets Rm
    setBits(&output, getRegNum(as->tokens[2]), 8);  //sets Rs

    setBits(&output, 9, 4);
    return output;
}

word evalMla(ASSEMBLY *as, STATE *state){
    word output = evalMul(as, state);
    setBits(&output, 1, ABit);
    setBits(&output, getRegNum(as->tokens[3]), 12); //sets Rn;
    return output;
}

word evalBranc(ASSEMBLY *as, STATE *state){
    word output = 0;
    setAlwaysCond(&output);
    setBits(&output, 5, 25);
    setBranchOffset(state, as, &output);
    return output;
}

word evalBNE(ASSEMBLY *as, STATE *state){
    word output = 0;
    setBits(&output, 1, flagV);
    setBits(&output, 1, 27);
    setBits(&output, 1, 25);
    setBranchOffset(state, as, &output);
    return output;
}

word evalBLT(ASSEMBLY *as, STATE *state){
    word output = 0;
    setBits(&output, 11, flagV);
    setBits(&output, 5, 25);
    setBranchOffset(state, as, &output);
    return output;
}

word evalBGE(ASSEMBLY *as, STATE *state){
    word output = 0;
    setBits(&output, 10, flagV);
    setBits(&output, 5, 25);
    setBranchOffset(state, as, &output);
    return output;
}

word evalBGT(ASSEMBLY *as, STATE *state){
    word output = 0;
    setBits(&output, 12, flagV);
    setBits(&output, 5, 25);
    setBranchOffset(state, as, &output);
    return output;
}

word evalBLE(ASSEMBLY *as, STATE *state){
    word output = 0;
    setBits(&output, 13, flagV);
    setBits(&output, 5, 25);
    setBranchOffset(state, as, &output);
    return output;
}

word evalBeq(ASSEMBLY *as, STATE *state){
    word output = 0;
    setBits(&output, 5, 25);
    setBranchOffset(state, as, &output);
    return output;
}

word evalLDR(ASSEMBLY *as, STATE *state) {
    if (isDirectLDR(as->tokens[1])) {
        word address = (word) decodeEXP(as->tokens[1]);
        if (address > 0xFF) {
            state->extras[state->noOfExtraLines] = address;
            as->noOfTokens = 3;
            as->tokens[1] = "[PC";
            char *test = malloc(30 * sizeof(char));
            test[0] = '#';
            test[1] = '\0';
            word offset = (((state->noOfLines + state->noOfExtraLines) * 4 - as->address) - 8);
            char str[10];
            sprintf(str, "%u", offset);
            strcat(test, str);
            strcat(test, "]");
            as->tokens[2] = test;
            printf("Printing new instructions:\n");
            for (int i = 0; i < as->noOfTokens; ++i) {
                printf("%s\n", as->tokens[i]);
            }
            state->noOfExtraLines ++;
            return evalLDR(as, state);
        } else {
            return evalMov(as, state);
        }
    } else {
        //pre/post indexed address
        word output = 0;
        setAlwaysCond(&output);
        setBits(&output, getRegNum(as->tokens[0]), 12);
        setBits(&output, 1, 26);
        setBits(&output, 1, SBit);
        processTransfers(as, state, &output);
        return output;
    }
}
word evalSTR(ASSEMBLY *as, STATE *state){
    word output = 0;
    setAlwaysCond(&output);
    setBits(&output, 1, 26);
    setBits(&output, getRegNum(as->tokens[0]), 12);
    processTransfers(as, state, &output);
    return output;
}

word evalLSL(ASSEMBLY *as, STATE *state){
    as->tokens[1][strlen(as->tokens[1])-1] = '\0';
    char *lsl = (char*) malloc(20 * sizeof(char));
    lsl[0] = '\0';
    strcat(lsl, "lsl");
    strcat(lsl, as->tokens[1]);
    as->tokens[2] = lsl;
    strcpy(as->tokens[1], as->tokens[0]);
    as->noOfTokens = 3;
    return evalMov(as, state);
}

word evalANDEQ(ASSEMBLY *as, STATE *state){
    return 0;
}

evalFunc functionLookup(char *lookUp){
    for(int i = 0; i < (sizeof(funcMap) / sizeof(funcMap[0])); i++ ){
        if(strcmp(lookUp, funcMap[i].name) == 0){
            printf("Match found for %s\n", funcMap[i].name);
            return funcMap[i].func;
        }
    }
    return NULL;
}

void assignLabels(STATE *state){
    FILE* source = fopen(state->input, "r");
    if (!source) {
        printf("Error occurred, failed to read file!");
        exit(EXIT_FAILURE);
    }
    address count = 0;
    char buffer[200];
    while (1) {
        memset(buffer, 0, strlen(buffer));
        fgets(buffer, 200, source);
        if (feof(source)) {
            break;
        }
        char *check = strchr(buffer, ':');
        //if starts with character and ends in :
        if (check != NULL) {
            int size = (int) (check-&buffer[0]);
            strncpy(state->labels[state->noOfLabels].label, buffer, (size_t) size);
            state->labels[state->noOfLabels].address = count;
            state->noOfLabels ++;
        } else {
            if (strcmp(buffer, "\n") != 0) {
                state->noOfLines++;
            count += 4;
            }
        }
    }
    fclose(source);
}

void writeToFile(STATE *state, word binary) {
    byte first = (byte) binary;
    byte second = (byte) (binary >> PC_OFFSET);
    byte third = (byte) (binary >> 16);
    byte last = (byte) (binary >> branchOffset);
    fwrite(&first, sizeof(byte),1,state->outputFile);
    fwrite(&second, sizeof(byte),1,state->outputFile);
    fwrite(&third, sizeof(byte),1,state->outputFile);
    fwrite(&last, sizeof(byte),1,state->outputFile);
}

void pass2(STATE *state) {
    FILE *input = fopen(state->input ,"r");
    //we know at this point first pass through worked so no further check needed
    char buffer[200];
    address address = 0;
    while (!feof(input)) {
        memset(buffer, 0, strlen(buffer));
        //clears the string
        fgets(buffer, 200, input);
        //copies next line into string
        if ((buffer[0] != 0) && (strcmp(buffer, "\n") != 0) && (strchr(buffer, ':') == NULL)) {
            //process the buffer
            ASSEMBLY *as = malloc(sizeof(ASSEMBLY));
            int n = 0;
            char* instruction;
            char* token;
            token = strtok(buffer, " ");
            instruction = token;
            token = strtok(NULL, ",");
            while (token != NULL) {
                RemoveSpaces(token);
                as->tokens[n] = token;
                token = strtok(NULL, ",");
                n++;
            }
            as->noOfTokens = n;
            as->address = address;
            word result = functionLookup(instruction)(as, state);
            printf("Output is %x\n", result);
            writeToFile(state, result);
            address += 4;
        }
    }
    fclose(input);
}

int main(int argc, char **argv) {

  if (argc != 3) {
      printf("Invalid arguments, please give input file and output file name");
      exit(EXIT_FAILURE);
  }

  char* input = argv[1];
  char* output = argv[2];

  STATE *state = calloc(1, sizeof(STATE));
  state->input = input;
  state->outputFile = fopen(output, "wb");
  assignLabels(state);

  //2nd pass through
    pass2(state);

    //write remaining ldr/str data

    for (int i = 0; i < state->noOfExtraLines; ++i) {
        writeToFile(state, state->extras[i]);
    }

    fclose(state->outputFile);
    free(state);
  return EXIT_SUCCESS;
}
