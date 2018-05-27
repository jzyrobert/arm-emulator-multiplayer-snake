#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>


typedef uint16_t address;
typedef uint32_t word;
typedef uint8_t byte;

typedef struct {
    char label[20];
    word address;
} LABEL;

typedef struct {
    LABEL labels[10];
    word extras[10];
    int noOfLines;
    int noOfLabels;
    int noOfExtraLines;
    char* input;
    char* output;
    FILE* outputFile;
} STATE;

typedef struct {
    char* tokens[6];
    word address;
    int noOfTokens;
} ASSEMBLY;

typedef word (*evalFunc)(ASSEMBLY *assembly, STATE *state);

typedef struct {
    char *name;
    evalFunc func;
} nameToFunc;

void setBits(word *output, word bits, word end);

void setAlwaysCond(word *output){
    setBits(output, 0xE, 28);
}

word getRegNum(char *name){
    name++;
    if (strchr(name, 'C')) {
        return 15;
    }
    return (word) strtol(name, NULL, 10);
}

void setBits(word *output, word bits, word end){
    //the bits are set leading to the end bit
    //end bit is the lowest bit of the section being set
    *output |= (bits << end);
}

void evalOperand2(ASSEMBLY *as, word *output, int op2Point);

word evalAdd(ASSEMBLY *as, STATE *state){
    word output = 0;
    setAlwaysCond(&output);
    setBits(&output, 4, 21);
    evalOperand2(as, &output, 2);
    setBits(&output, getRegNum(as->tokens[0]), 12);
    setBits(&output, getRegNum(as->tokens[1]), 16);
    return output;
}

word evalSub(ASSEMBLY *as, STATE *state){
    word output = 0;
    setAlwaysCond(&output);
    setBits(&output, 2, 21);
    setBits(&output, getRegNum(as->tokens[0]), 12);
    setBits(&output, getRegNum(as->tokens[1]), 16);
    evalOperand2(as, &output, 2);
    return output;
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
    address = (word) ((int32_t) address - (int32_t) as->address - 8 >> 2 & ((1 << 25) - 1));
    return address;
}

void setBranchOffset(STATE *state, ASSEMBLY *as, word *output) {
    word offset = calculateBOffset(state, as,as->tokens[0]);
    offset &= 0xFFFFFF;
    setBits(output, offset, 0);
}

word evalBranc(ASSEMBLY *as, STATE *state){
    word output = 0;
    setAlwaysCond(&output);
    setBits(&output, 5, 25);
    setBranchOffset(state, as, &output);
    return output;
}

word evalMov(ASSEMBLY *as, STATE *state){
    word output = 0;
    setAlwaysCond(&output);
    setBits(&output, 13, 21);
    setBits(&output, getRegNum(as->tokens[0]), 12);
    evalOperand2(as, &output, 1);
    return output;
}

void stripBrackets(char* string) {
    size_t len = strlen(string);
    memmove(string, string+1, len-2);
    string[len-2] = 0;
}

long decodeEXP(char *str) {
    int offset = 0;
    if (strchr(str, '-') != NULL) {
        offset++;
    }
    if (strchr(str, 'x') != NULL) {
        offset +=3;
        return strtol(str + offset , NULL, 16);
    } else {
        offset++;
        return strtol(str + offset, NULL, 10);
    }
}

void evalShifts (ASSEMBLY *as, word *output, int op2Point) ;

void processTransfers(ASSEMBLY *as, STATE *state, word *output) {
    printf("Processing index assembly:\n");
    if (strchr(as->tokens[1], ']') != NULL) {
        //[RN]- something?
        if (as->noOfTokens == 2) {
            //pre-index RN
            stripBrackets(as->tokens[1]);
            setBits(output, getRegNum(as->tokens[1]), 16);
            setBits(output, 1, 24);
            setBits(output, 1, 23);
        } else {
            //post index [RN], expression
            if (strchr(as->tokens[2], 'r') != NULL) {
                setBits(output, 1, 25);
            }
            stripBrackets(as->tokens[1]);
            setBits(output, 1, 23);
            setBits(output, getRegNum(as->tokens[1]), 16);
            setBits(output, (word) decodeEXP(as->tokens[2]), 0);
        }
    } else {
        //[Rn - Expression]
        setBits(output, 1, 24);
        //Sets P
        if (strchr(as->tokens[2], '-') == NULL) {
            //its not negative, sets U;
            setBits(output, 1, 23);
        }
        //Setting RN
        setBits(output,getRegNum(as->tokens[1] + 1) , 16);
        if (as->noOfTokens == 3) {
            if (strchr(as->tokens[2], 'r') != NULL) {
                setBits(output, 1, 25);
            }
            as->tokens[2][strlen(as->tokens[2]) - 1] = '\0';
            setBits(output, (word) decodeEXP(as->tokens[2]), 0);
        } else {
            //optional shift case
            setBits(output, 1, 25);
            if (strchr(as->tokens[2], '-') != NULL) {
                //if negative remove sign before processing;
                as->tokens[2] = as->tokens[2] + 1;
            }
            as->tokens[3][strlen(as->tokens[3])-2] = '\n';
            as->tokens[3][strlen(as->tokens[3])-1] = '\0';
            evalShifts(as, output, 2);
        }
    }
}

word evalLDR(ASSEMBLY *as, STATE *state) {
    if (strchr(as->tokens[1], '=') != NULL) {
        word address = (word) strtol(as->tokens[1] + 1, NULL,0);
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
    setBits(&output, 1, 20);
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

word evalBNE(ASSEMBLY *as, STATE *state){
    word output = 0;
    setBits(&output, 1, 28);
    setBits(&output, 1, 27);
    setBits(&output, 1, 25);
    setBranchOffset(state, as, &output);
    return output;
}

word evalBLT(ASSEMBLY *as, STATE *state){
    word output = 0;
    setBits(&output, 11, 28);
    setBits(&output, 5, 25);
    setBranchOffset(state, as, &output);
    return output;
}

word evalBGE(ASSEMBLY *as, STATE *state){
    word output = 0;
    setBits(&output, 10, 28);
    setBits(&output, 5, 25);
    setBranchOffset(state, as, &output);
    return output;
}

word evalBGT(ASSEMBLY *as, STATE *state){
    word output = 0;
    setBits(&output, 12, 28);
    setBits(&output, 5, 25);
    setBranchOffset(state, as, &output);
    return output;
}

word evalBLE(ASSEMBLY *as, STATE *state){
    word output = 0;
    setBits(&output, 13, 28);
    setBits(&output, 5, 25);
    setBranchOffset(state, as, &output);
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

word evalANDEQ(){
    return 0;
}
word evalBeq(ASSEMBLY *as, STATE *state){
    word output = 0;
    setBits(&output, 5, 25);
    setBranchOffset(state, as, &output);
    return output;
}

word evalRsb(ASSEMBLY *as, STATE *state){
    word output = 0;
    setAlwaysCond(&output);
    setBits(&output, getRegNum(as->tokens[0]), 12);
    setBits(&output, getRegNum(as->tokens[1]), 16);
    setBits(&output, 3, 21);
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
    setBits(&output, 1, 21);
    evalOperand2(as, &output, 2);;
    return output;
}

word evalOrr(ASSEMBLY *as, STATE *state){
    word output = 0;
    setAlwaysCond(&output);
    setBits(&output, getRegNum(as->tokens[0]), 12);
    setBits(&output, getRegNum(as->tokens[1]), 16);
    setBits(&output, 12, 21);
    evalOperand2(as, &output, 2);
    return output;
}

word evalTst(ASSEMBLY *as, STATE *state){
    word output = 0;
    setAlwaysCond(&output);
    setBits(&output, 1, 20);
    setBits(&output, 8, 21);
    setBits(&output, getRegNum(as->tokens[0]), 16);
    evalOperand2(as, &output, 1);
    return output;
}

word evalTeq(ASSEMBLY *as, STATE *state){
    word output = 0;
    setAlwaysCond(&output);
    setBits(&output, 1, 20);
    setBits(&output, 9, 21);
    setBits(&output, getRegNum(as->tokens[0]), 16);
    evalOperand2(as, &output, 1);
    return output;
}

word evalCmp(ASSEMBLY *as, STATE *state){
    word output = 0;
    setAlwaysCond(&output);
    setBits(&output, 1, 20);
    setBits(&output, 10, 21);
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
    setBits(&output, 1, 21);
    setBits(&output, getRegNum(as->tokens[3]), 12); //sets Rn;
    return output;
}

int rangeOfBits(word imm);

int lowestBit(word imm) ;

void evalExp (ASSEMBLY *as, word *output, int op2Point);

int isEven(word num) {
    return !(num % 2);
}

void evalOperand2(ASSEMBLY *as, word *output, int op2Point){

    if ((strchr(as->tokens[op2Point], '#') != NULL) || (strchr(as->tokens[op2Point], 'x') != NULL)){
        evalExp(as, output, op2Point);
    } else {
         evalShifts(as, output, op2Point);
    }
}

void evalExp (ASSEMBLY *as, word *output, int op2Point){
    stripBrackets(as->tokens[op2Point]);

    setBits(output, 1, 25);
    word imm;

    if (strchr(as->tokens[op2Point], 'x')) {
        imm = (word) strtol(as->tokens[op2Point] + 2, NULL, 16);
    } else {
        imm = (word) strtol(as->tokens[op2Point], NULL, 10);
    }
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


void evalShifts (ASSEMBLY *as, word *output, int op2Point) {
    if ((as->noOfTokens - op2Point) < 2) {
        //no shift
        as->tokens[op2Point][strlen(as->tokens[op2Point]) - 1] = '\0';
        setBits(output, getRegNum(as->tokens[op2Point]), 0);
    } else {
        //shift
        setBits(output, (word) strtol(as->tokens[op2Point] + 1, NULL, 10), 0);
        if (strchr(as->tokens[op2Point + 1], 'l') != NULL) {
            //lsl or lsr
            if (as->tokens[op2Point+1][2] == 'r') {
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
        if (strchr(as->tokens[op2Point + 1], '#')) {
            if (strchr(as->tokens[op2Point+1], 'x')) {
                setBits(output, (word) strtol(as->tokens[op2Point + 1] + 6, NULL, 16), 7);
            } else {
                setBits(output, (word) strtol(as->tokens[op2Point + 1] + 4, NULL, 10), 7);
            }
        } else {
            setBits(output,1 ,4);
            setBits(output, getRegNum(as->tokens[op2Point + 1] + 3),8);
        }
    }
}


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
    while (!feof(source)) {
        memset(buffer, 0, strlen(buffer));
        fgets(buffer, 200, source);
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
    byte second = (byte) (binary >> 8);
    byte third = (byte) (binary >> 16);
    byte last = (byte) (binary >> 24);
    fwrite(&first, sizeof(byte),1,state->outputFile);
    fwrite(&second, sizeof(byte),1,state->outputFile);
    fwrite(&third, sizeof(byte),1,state->outputFile);
    fwrite(&last, sizeof(byte),1,state->outputFile);
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
            word result = (functionLookup(instruction))(as, state);
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
  state->output = output;
  state->outputFile = fopen(state->output, "wb");
  assignLabels(state);

  //2nd pass through
    pass2(state);

    //write remaining ldr/str data

    for (int i = 0; i < state->noOfExtraLines; ++i) {
        writeToFile(state, state->extras[i]);
    }

    fclose(state->outputFile);

  return EXIT_SUCCESS;
}
