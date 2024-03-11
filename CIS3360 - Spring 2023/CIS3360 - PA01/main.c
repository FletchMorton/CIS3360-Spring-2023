/*=============================================================================
| Assignment: pa01 - Encrypting a plaintext file using the Vigenere cipher
|
| Author: Fletcher Morton
| Language: C
|
| To Compile: gcc -o pa01 pa01.c
|
| To Execute: c -> ./pa01 kX.txt pX.txt
| where kX.txt is the keytext file
| and pX.txt is plaintext file
|
| Note: All input files are simple 8 bit ASCII input
|
| Class: CIS3360 - Security in Computing - Spring 2023
| Instructor: McAlpin
| Due Date: March 6th 2023
|
+=============================================================================*/

//Include/Define
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#define INPUT_MAX   10000

//Prototypes
void sendERROR(char *);
void readKey(char *, int *);
void readInput(char *, char *, int);
void multMatrices(char *, int);

//Global Variables
int **keyMatrix = NULL; //A 2D array to hold the key matrix


int main(int argc, char **argv)
{
    //Variables
    int size = 0;                //Number of rows and columns in the encryption matrix (1 < n < 10)
    char ptInput[INPUT_MAX];     //Array holding only the lowercase alphabetic characters from the input

    //Assert that there were no errors when passing/receiving the files
    assert(argc != 0 && argv != NULL);

    //Read in the key file
    readKey(argv[1], &size);

    //Read in the plaintext file
    readInput(argv[2], ptInput, size);

    //Encrypt the plaintext
    multMatrices(ptInput, size);

    //Return
    return EXIT_SUCCESS;
}

//_________________________________________________________________

//Read in the key file and initialize the key matrix
void readKey(char *filename, int *size)
{
    FILE *input = NULL;
    int x;

    //Check for file name error
    if(filename == NULL)
        sendERROR("An error occurred while passing the filename.\n");

    //Try to connect the FILE pointer to the key file
    input = fopen(filename, "r");
    if(input == NULL)
        sendERROR("The passed file does not exist.\n");

    //Read in the matrix size and set up the key matrix
    fscanf(input, "%d", size);
    keyMatrix = (int **) calloc(*size, sizeof(int *));

    fprintf(stdout, "\nKey matrix:\n");

    //Read in the key matrix
    for(int i = 0; i < *size; i++) {

        keyMatrix[i] = (int *) calloc(*size, sizeof(int));
        for(int j = 0; j < *size; j++) {

            //If file ends before the expected input was retrieved, send error
            if(feof(input)) sendERROR("Specified size and matrix size are not equal");

            //Grabs the next integer
            fscanf(input, "%d", &x);

            //Add integer to the matrix
            keyMatrix[i][j] = x;
            fprintf(stdout, "%4d", keyMatrix[i][j]);
        }

        fprintf(stdout, "\n");
    }

}



//Read in the input file
void readInput(char *filename, char *ptInput, int size)
{
    FILE *input = NULL;
    char c;

    //Check for file name error
    if(filename == NULL)
        sendERROR("An error occurred while passing the filename.\n");

    //Try to connect the FILE pointer to the key file
    input = fopen(filename, "r");
    if(input == NULL)
        sendERROR("The passed file does not exist.\n");

    fprintf(stdout, "\nPlaintext:\n");

    //Read in the plaintext input
    int i = 0;
    while(!feof(input)) {
        //Send error if the maximum character limit was exceeded
        if(i == INPUT_MAX) sendERROR("Plaintext exceeds the maximum character limit.\n");

        //Grab the next character
        c = getc(input);

        //If the character is alphabetic, add it to the string. Other characters get ignored
        if((c > 64 && c < 91) || (c > 96 && c < 123)) {

            //If uppercase, make lowercase
            if(c < 96) c = tolower(c);

            //Add to plaintext input string
            ptInput[i] = c;

            //Print to screen
            fprintf(stdout, "%c", ptInput[i]);
            i++;

            if(i%80 == 0) fprintf(stdout, "\n");
        }

    }

    //Pad with 'x' characters
    if(i%size != 0) {
        for(int j = 0; j < (size - (i%size)); j++) { //Pad with the appropriate amount of 'x' characters
            ptInput[j+i] = 'x';
            fprintf(stdout, "%c", ptInput[j+i]);
            if((i+j+1)%80 == 0) fprintf(stdout, "\n");
            if(j+1 == (size - (i%size))) ptInput[j+i+1] = '\0';
        }
    } else ptInput[i] = '\0';


    fprintf(stdout, "\n");
}



//Multiply each block of the plaintext by the key, and store the product in each block of the ciphertext
void multMatrices(char *ptInput, int size)
{
    //Initialize the plaintext and ciphertext block matrices
    int *ptMatrix = (int *) calloc(size, sizeof(int));
    int *ctMatrix = (int *) calloc(size, sizeof(int));

    fprintf(stdout, "\nCiphertext:");

    for(int i = 0; i < INPUT_MAX; i += size) { //For each block of plaintext

        if(ptInput[i] == '\0') break;

        //Update the plaintext matrix
        for(int j = 0; j < size; j++)
            ptMatrix[j] = (int) ptInput[i+j] - 97;

        for(int j = 0; j < size; j++) { //For each row in the key
            ctMatrix[j] = 0;

            for(int k = 0; k < size; k++) //For each digit in the key
                ctMatrix[j] += keyMatrix[j][k] * ptMatrix[k]; //Multiply

            if((i+j)%80 == 0) fprintf(stdout, "\n");

            //Modulo product by 26
            ctMatrix[j] %= 26;

            //Convert the value into back into a letter
            fprintf(stdout, "%c", ctMatrix[j] + 97);

        }

    }


    fprintf(stdout, "\n");
    return;
}


//If an error is encountered, this function is passed a string, and outputs the string to stderr before exiting
void sendERROR(char *errMsg)
{
    fflush(stdout);
    fprintf(stderr, "%s", errMsg);
    exit(EXIT_FAILURE);
}

/*=============================================================================
| I Fletcher Morton (fl826999) affirm that this program is
| entirely my own work and that I have neither developed my code together with
| any another person, nor copied any code from any other person, nor permitted
| my code to be copied or otherwise used by any other person, nor have I
| copied, modified, or otherwise used programs created by others. I acknowledge
| that any violation of the above terms will be treated as academic dishonesty.
+=============================================================================*/
