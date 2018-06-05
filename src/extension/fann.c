#include "fannF/src/include/fann.h"

int main(int argc, char* argv[]) {
    char* input;
    char* output;
    int epochs = 0, tA = 0;
    if (argc != 5) {
        printf("Enter input, output, number of max epochs and corresponding training algorithm(0-4 use 2 if unsure)!");
        exit(EXIT_FAILURE);
    } else {
        input = argv[1];
        output = argv[2];
        epochs = (unsigned int) strtol(argv[3], NULL ,10);
        tA = (unsigned int) strtol(argv[4], NULL, 10);
    }
    const unsigned int num_input = 8;
    const unsigned int num_output = 1;
    const unsigned int num_layers = 3;
    const unsigned int num_neurons_hidden = 20;
    const float desired_error = (const float) 0.001;
    const unsigned int epochs_between_reports = 100;

    struct fann *ann = fann_create_standard(num_layers, num_input,
                                            num_neurons_hidden, num_output);
    fann_set_training_algorithm(ann, (enum fann_train_enum) tA);
    fann_set_activation_function_output(ann, FANN_SIGMOID_SYMMETRIC);

    fann_train_on_file(ann, input, epochs,
                       epochs_between_reports, desired_error);

    fann_save(ann, output);

    fann_destroy(ann);

    return 0;
}
