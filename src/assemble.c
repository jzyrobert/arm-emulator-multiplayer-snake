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
    int noOfLabels;
    char* input;
    char* output;
    FILE* outputFile;
} STATE;

typedef word (*evalFunc)(char **line, STATE *state);

typedef struct {
    char *name;
    evalFunc func;
} nameToFunc;

word evalAdd(char **line, STATE *state){
    return 0;
}

word evalSub(char **line, STATE *state){
    return 0;
}

word evalBranc(char **line, STATE *state){
    return 0;
}

word evalLDR(char **line, STATE *state){
    return 0;
}
word evalSTR(char **line, STATE *state){
    return 0;
}

word evalBNE(char **line, STATE *state){
    return 0;
}

word evalBLT(char **line, STATE *state){
    return 0;
}

word evalBGE(char **line, STATE *state){
    return 0;
}

word evalBGT(char **line, STATE *state){
    return 0;
}

word evalBLE(char **line, STATE *state){
    return 0;
}

word evalLSL(char **line, STATE *state){
    return 0;
}

word evalANDEQ(char **line, STATE *state){
    return 0;
}
word evalBeq(char **line, STATE *state){
    return 0;
}

word evalRsb(char **line, STATE *state){
    return 0;
}

word evalAnd(char **line, STATE *state){
    return 0;
}

word evalEor(char **line, STATE *state){
    return 0;
}

word evalOrr(char **line, STATE *state){
    return 0;
}

word evalMov(char **line, STATE *state){
    return 0;
}

word evalTst(char **line, STATE *state){
    return 0;
}

word evalTeq(char **line, STATE *state){
    return 0;
}

word evalCmp(char **line, STATE *state){
    return 0;
}

word evalMul(char **line, STATE *state){
    return 0;
}

word evalMla(char **line, STATE *state){
    return 0;
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
            count += 4;
        }
    }
    fclose(source);
    for (int i = 0; i < state->noOfLabels; ++i) {
        printf("Label: %s\n", state->labels[i].label);
        printf("Label address: 0x%08x\n", state->labels[i].address);
    }
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
    while (!feof(input)) {
        memset(buffer, 0, strlen(buffer));
        //clears the string
        fgets(buffer, 200, input);
        //copies next line into string
        if ((buffer[0] != 0) && (strcmp(buffer, "\n") != 0) && (strchr(buffer, ':') == NULL)) {
            //process the buffer
            int n = 1;
            char *tokens[6];
            char* token;
            token = strtok(buffer, " ");
            tokens[0] = token;
            token = strtok(NULL, ",");
            while (token != NULL) {
                RemoveSpaces(token);
                tokens[n] = token;
                token = strtok(NULL, ",");
                n++;
            }
            word result = (functionLookup(tokens[0]))(tokens + 1, state);
            for (int i = 0; i < n; ++i) {
                printf("The %dth value is %s\n",i, tokens[i]);
            }
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



    fclose(state->outputFile);

  return EXIT_SUCCESS;
}


