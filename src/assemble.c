#include <stdlib.h>
#include <stdio.h>
#include <mem.h>
#include <stdint.h>


typedef uint16_t address;

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
  state->outputFile = fopen(state->output, "rw");
  assignLabels(state);

  //2nd pass through

  return EXIT_SUCCESS;
}
