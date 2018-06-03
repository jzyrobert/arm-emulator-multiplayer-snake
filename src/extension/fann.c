#include "fannF/src/include/fann.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Enter number of max epochs!");
        exit(EXIT_FAILURE);
    }
    const unsigned int num_input = 8;
    const unsigned int num_output = 1;
    const unsigned int num_layers = 3;
    const unsigned int num_neurons_hidden = 25;
    const float desired_error = (const float) 0.001;
    const unsigned int epochs_between_reports = 1000;

    struct fann *ann = fann_create_standard(num_layers, num_input,
                                            num_neurons_hidden, num_output);

    fann_train_on_file(ann, "updatetraining.data", (unsigned int) strtol(argv[1], NULL, 10),
                       epochs_between_reports, desired_error);

    fann_save(ann, "updatetraining.net");

    fann_destroy(ann);

    return 0;
}
