#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>


typedef uint16_t address;
typedef uint32_t word;
typedef uint8_t byte;

typedef struct {
    char label[20];
    address address;
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
    address address;
    int noOfTokens;
} ASSEMBLY;

typedef word (*evalFunc)(ASSEMBLY *assembly, STATE *state);

typedef struct {
    char *name;
    evalFunc func;
} nameToFunc;

void setAlwaysCond(word *output){
    *output |= (0xE << 28);
}

word getRegNum(char *name){
    name++;
    return (word) atoi(name);
}

void setBits(word *output, word bits, word end){
    //the bits are set leading to the end bit
    //end bit is the lowest bit of the section being set
    *output |= (bits << end);
}

word evalAdd(ASSEMBLY *as, STATE *state){
    word output = 0;
    setAlwaysCond(&output);
    byte op2 = (byte) strtol(as->tokens[2] + 1, NULL, 10);
    long rn = strtol(as->tokens[1] + 1, NULL, 10);
    long rd = strtol(as->tokens[0] + 1, NULL, 10);

    output |= (0x00800000);
    output |= (rd << 16);
    output |= (rn << 12);
    output |= op2;

    //printf("Output is %x\n", output);
    return output;
}

word evalSub(ASSEMBLY *as, STATE *state){
    word output = 0;

    setAlwaysCond(&output);
    byte op2 = (byte) strtol(as->tokens[2] + 1, NULL, 10);
    long rn = strtol(as->tokens[1] + 1, NULL, 10);
    long rd = strtol(as->tokens[0] + 1, NULL, 10);

    output |= (0x00400000);
    output |= (rd << 16);
    output |= (rn << 12);
    output |= op2;

    //printf("Output is %x\n", output);
    return output;
}

word evalBranc(ASSEMBLY *as, STATE *state){
    return 0;
}

word evalMov(ASSEMBLY *as, STATE *state){
    word output = 0;
    setAlwaysCond(&output);
    byte op2;
    printf("%s\n", as->tokens[1]);
    if (strchr(as->tokens[1], 'x') != NULL) {
      op2 = (byte) strtol(as->tokens[1] + 3, NULL, 16);
    } else {
      op2 = (byte) strtol(as->tokens[1] + 1, NULL, 10);
    }
    printf("%d\n", op2);
    long rn = strtol(as->tokens[0] + 1, NULL, 10);
    output |= (1 << 25);
    output |= (13 << 21);
    output |= (rn << 12);
    output |= op2;
    //printf("Output is %x\n", output);
    return output;
}

void processTransfers(ASSEMBLY *as, STATE *state, word *output) {

}

word evalLDR(ASSEMBLY *as, STATE *state) {
    if (strchr(as->tokens[1], '=') != NULL) {
        word address = (word) strtol(as->tokens[1] + 1, NULL,0);
        if (address > 0xFF) {
            state->extras[state->noOfExtraLines] = address;
            as->noOfTokens = 4;
            as->tokens[2] = "[PC";
            as->tokens[3] = "#";
            word offset = ((state->noOfLines * 4 - as->address) - 8);
            char str[10];
            sprintf(str, "%u", offset);
            strcat(as->tokens[3], str);
            strcat(as->tokens[3], "]");
            printf("Printing new instructions:\n");
            for (int i = 0; i < 4; ++i) {
                printf("%s\n", as->tokens[i]);
            }
            state->extras[state->noOfExtraLines] ++;
            return evalLDR(as, state);
        } else {
            return evalMov(as, state);
        }
    } else {
        //pre/post indexed address
    word output = 0;
    setAlwaysCond(&output);
    long rd = strtol(as->tokens[0] + 1, NULL, 10);
    output |= (rd << 12);
    output |= (1 << 26);
    output |= (1 << 20);
    processTransfers(as, state, &output);
    return output;
    }
}
word evalSTR(ASSEMBLY *as, STATE *state){
    return 0;
}

word evalBNE(ASSEMBLY *as, STATE *state){
    return 0;
}

word evalBLT(ASSEMBLY *as, STATE *state){
    return 0;
}

word evalBGE(ASSEMBLY *as, STATE *state){
    return 0;
}

word evalBGT(ASSEMBLY *as, STATE *state){
    return 0;
}

word evalBLE(ASSEMBLY *as, STATE *state){
    return 0;
}

word evalLSL(ASSEMBLY *as, STATE *state){
    return 0;
}

word evalANDEQ(ASSEMBLY *as, STATE *state){
    return 0;
}
word evalBeq(ASSEMBLY *as, STATE *state){
    return 0;
}

word evalRsb(ASSEMBLY *as, STATE *state){
    word output = 0;

    setAlwaysCond(&output);
    byte op2 = (byte) strtol(as->tokens[2] + 1, NULL, 10);
    long rn = strtol(as->tokens[1] + 1, NULL, 10);
    long rd = strtol(as->tokens[0] + 1, NULL, 10);

    output |= (0x00600000);
    output |= (rd << 16);
    output |= (rn << 12);
    output |= op2;

    //printf("Output is %x\n", output);
    return output;
}

word evalAnd(ASSEMBLY *as, STATE *state){
    word output = 0;

    setAlwaysCond(&output);
    byte op2 = (byte) strtol(as->tokens[2] + 1, NULL, 10);
    long rn = strtol(as->tokens[1] + 1, NULL, 10);
    long rd = strtol(as->tokens[0] + 1, NULL, 10);

    output |= (rd << 16);
    output |= (rn << 12);
    output |= op2;

    //printf("Output is %x\n", output);
    return output;
}

word evalEor(ASSEMBLY *as, STATE *state){
    word output = 0;


    setAlwaysCond(&output);
    byte op2 = (byte) strtol(as->tokens[2] + 1, NULL, 10);
    long rn = strtol(as->tokens[1] + 1, NULL, 10);
    long rd = strtol(as->tokens[0] + 1, NULL, 10);

    output |= (0x00200000);
    output |= (rd << 16);
    output |= (rn << 12);
    output |= op2;

    //printf("Output is %x\n", output);
    return output;
}

word evalOrr(ASSEMBLY *as, STATE *state){
    return 0;
}

/* redundant evaladd test code
word output = 0;
output |= (0xE << 28);
byte op2;
if (strchr(as->tokens[2], 'x') != NULL) {
  op2 = (byte) strtol(as->tokens[2] + 3, NULL, 16);
} else {
  op2 = (byte) strtol(as->tokens[2] + 1, NULL, 10);
}
if (strchr(as->tokens[2], 'r') == NULL) {
   output |= (1 << 25);
}
long rd = strtol(as->tokens[0] + 1, NULL, 10);
long rn = strtol(as->tokens[1] + 1, NULL, 10);
output |= (4 << 21);
output |= (rn << 16);
output |= (rd << 12);
output |= op2;
return output;
*/

word evalTst(ASSEMBLY *as, STATE *state){
    word output = 0;

    setAlwaysCond(&output);
    byte op2 = (byte) strtol(as->tokens[1] + 1, NULL, 10);
    long rn = strtol(as->tokens[0] + 1, NULL, 10);

    output |= (0x01100000);
    output |= (rn << 12);
    output |= op2;

    //printf("Output is %x\n", output);
    return output;
}

word evalTeq(ASSEMBLY *as, STATE *state){
    word output = 0;

    setAlwaysCond(&output);
    byte op2 = (byte) strtol(as->tokens[1] + 1, NULL, 10);
    long rn = strtol(as->tokens[0] + 1, NULL, 10);

    output |= (0x01300000);
    output |= (rn << 12);
    output |= op2;

    //printf("Output is %x\n", output);
    return output;
}

word evalCmp(ASSEMBLY *as, STATE *state){
    return 0;
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
        fgets(buffer, 200, source);
        char *check = strchr(buffer, ':');
        //if starts with character and ends in :
        if (check != NULL) {
            int size = (int) (check-&buffer[0]);
            strncpy(state->labels[state->noOfLabels].label, buffer, size);
            state->labels[state->noOfLabels].address = count;
            state->noOfLabels ++;
        } else {
            state->noOfLines ++;
            count += 4;
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

void RemoveSpaces(char* source)
{
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
