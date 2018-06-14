# Programming III: Group 1 C Group Project #

Welcome to our repository for our solutions to the C Group Project.

## Running the extension ##
The extension is found in `extension`. After running `make`, running the generated file called `snake` will start the game. This will only work if you have the C library FANN dynamically installed on your system. You will receive an error message otherwise.

Alternatively, calling `make run` will also link it to the included static library on systems without the FANN library.

Do note that the game also requires the Ncurses library and will only run on UNIX compatible systems.

Choosing the "start server" option will allow LAN connections from any other device through the displayed IP address and port, enabling network controls

If you have an Android device, you can similarly install the [Snake Wars Controller App](https://play.google.com/store/apps/details?id=com.cproject.group1.snakewars) and connect as above.

Finally, running `demo` or `make rundemo` will provide you with a fun non-interactive simulation in your terminal for you to enjoy watching.

## Licenses ##
Please note that the folder `extension/fannF` contains the [Fast Artificial Neural Network Library](http://leenissen.dk/fann/wp/) for static usage. 

We did not write this, and you can find the license under which we have used it in the `fannF` directory.
