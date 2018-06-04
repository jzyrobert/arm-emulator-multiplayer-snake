#include "fannF/src/include/fann.h"

int main(int argc, char* argv[]) {
    char* input;
    char* output;
    int epochs = 0;
    if (argc != 4) {
        printf("Enter input, output and number of max epochs!");
        exit(EXIT_FAILURE);
    } else {
        input = argv[1];
        output = argv[2];
        epochs = (unsigned int) strtol(argv[3], NULL ,10);
    }
    const unsigned int num_input = 8;
    const unsigned int num_output = 1;
    const unsigned int num_layers = 3;
    const unsigned int num_neurons_hidden = 5;
    const float desired_error = (const float) 0.001;
    const unsigned int epochs_between_reports = 100;

    struct fann *ann = fann_create_standard(num_layers, num_input,
                                            num_neurons_hidden, num_output);

    fann_train_on_file(ann, input, epochs,
                       epochs_between_reports, desired_error);

    fann_save(ann, output);

    fann_destroy(ann);

    return 0;
}
