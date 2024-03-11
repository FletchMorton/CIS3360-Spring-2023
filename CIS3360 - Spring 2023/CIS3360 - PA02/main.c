/*=============================================================================
| Assignment: pa02 - Calculating an 8, 16, or 32 bit
| checksum on an ASCII input file
|
| Author: Fletcher Morton
| Language: c
|
| To Compile: gcc -o pa02 pa02.c
|
| To Execute: c -> ./pa02 inputFile.txt 8
| where inputFile.txt is an ASCII input file
| and the number 8 could also be 16 or 32
| which are the valid checksum sizes, all
| other values are rejected with an error message
| and program termination
|
| Note: All input files are simple 8 bit ASCII input
|
| Class: CIS3360 - Security in Computing - Spring 2023
| Instructor: McAlpin
| Due Date: April 23rd 2023
|
+=============================================================================*/

//Include/Define
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#define INPUT_MAX   10000      //Input may not exceed 10000 characters
#define HEX_MAX     2          //ASCII 255 or hex FF
#define MASK8       0xFF       //Show the last 2 bits
#define MASK16      0xFFFF     //Show the last 4 bits
#define MASK32      0xFFFFFFFF //Show the last 8 bits

//Prototypes
void *readInput(char *, int);
void calculateChecksum(int);
char *intToHexString(int);
int hexStringToInt(char *, int);
void sendERROR(char *);
int64_t getMask(int);
int powerOf(int, int);
char toHexRep(int);
int hexToDecimal(char);


//Global Variables
int inputArr[INPUT_MAX]; //Integer array filled with the ASCII values of each character retrieved from the input file
int characterCnt;        //Number of characters in the input


//Driver Function
int main(int argc, char **argv)
{
    //Assert that there were no errors when passing/receiving the files
    assert(argc != 0 && argv != NULL);

    //Read in the input file and store the ASCII values of each character in inputArr
    readInput(argv[1], atoi(argv[2]));

    //Calculate the checksum
    calculateChecksum(atoi(argv[2]));

    //Return
    return EXIT_SUCCESS;
}

//Functions
//_________________________________________________________________


//Read in the input file, placing the ASCII value of each character encountered into the global array
void *readInput(char *filename, int size)
{
    //Variables
    FILE *input = NULL;
    char c;

    //Check for file name error
    if(filename == NULL)
        sendERROR("An error occurred while passing the filename.\n");

    //Try to connect the FILE pointer to the input file
    input = fopen(filename, "r");
    if(input == NULL)
        sendERROR("The passed file does not exist.\n");

    //Formatting
    fprintf(stdout, "\n");

    //Read in the input file
    while(!feof(input)) {
        c = getc(input); //Grab next character

        //Allow all characters except the EOF char
        if(c != -1) {
            //Print to screen
            fprintf(stdout, "%c", c);
            //Add value to the input array
            inputArr[characterCnt] = c;
            //Increment character count
            characterCnt++;
            //Formatting
            if(characterCnt%80 == 0) fprintf(stdout, "\n");
        }
    }

    //Pad with the necessary amount of 'X' characters
    while(2*characterCnt%(size/4) != 0) {
        fprintf(stdout, "X");
        inputArr[characterCnt] = 'X';
        characterCnt++;

        if(characterCnt%80 == 0) fprintf(stdout, "\n"); //Formatting
    }

    //Formatting
    fprintf(stdout, "\n");
}


//Calculate the checksum according to the provided size and finally display the checksum in hex
void calculateChecksum(int size)
{
    //Variables
    int64_t checksum = 0;
    uint64_t mask = getMask(size); //Formatting. How many bits will be shown or hidden when displaying the checksum
    char block[size/4]; //Different size block for each size (8, 16, 32). Effects checksum calculation
    int blockPt = 0;

    //For each character in the input
    for(int i = 0; i < characterCnt; i++) {
        char *hexRep = intToHexString(inputArr[i]); //hexRep is only of size 2 since all characters (to ASCII 255 aka hex FF) will only have 2 digits in hex

        //Add the two characters of the int's hex representation to the current block
        block[blockPt] = hexRep[0];
        blockPt++;
        block[blockPt] = hexRep[1];
        blockPt++;

        //Once the appropriate amount of digits are in the block, add that block's value to the checksum
        if(blockPt == size/4) {
            checksum += hexStringToInt(block, size);
            blockPt = 0;
        }
    }

    //Print result
    fprintf(stdout, "%2d bit checksum is %8lx for all %4d chars\n", size, checksum & mask, characterCnt);
}


//Convert an integer value from decimal to hexadecimal
char *intToHexString(int num)
{
    //Allocate an array of size HEX_MAX (2)
    char *hex = (char *) calloc(HEX_MAX, sizeof(char));

    //Calculate the value
    hex[1] = toHexRep(num%16); //Tens place
    hex[0] = toHexRep(num/16); //Ones place

    //Return the resulting string
    return hex;
}


//Convert a string representing a hexadecimal value into an integer representing that same value in decimal
int hexStringToInt(char *hexStr, int size)
{
    //Have an integer to store the value
    int result = 0;

    //Apply the conversion method
    for(int i = 0; i < size/4; i++)
        result += (hexToDecimal(hexStr[i]) * powerOf(16, (size/4)-(i+1))); //64 -> (6 * 16^1) + (4 * 16^0) for example

    //Return the resulting integer value
    return result;
}

//If an error is encountered, this function is passed a string, and outputs the string to stderr before exiting
void sendERROR(char *errMsg)
{
    fflush(stdout);
    fprintf(stderr, "%s", errMsg);
    exit(EXIT_FAILURE);
}


//Utility Functions
//_________________________________________________________________

//Run a switch on the passed size to determine which mask value to return
int64_t getMask(int size)
{
    switch(size) {
        case 8: return MASK8;
        case 16: return MASK16;
        case 32: return MASK32;
        default: sendERROR("Some bad size was passed to function getMask.\n");
    }

    //End of function return
    return MASK32;
}

//Function to preform exponent calculations since the test script does not support the use of the <math.h> library
int powerOf(int base, int exponent)
{
    //Base Cases
    if(exponent == 0) return 1;
    if(exponent == 1) return base;

    //Multiplication
    int result = base;
    for(int i = 1; i < exponent; i++)
        result *= base;

    //Return result
    return result;
}


//Run a switch on the passed integer value to determine which character to return
char toHexRep(int value)
{
    switch(value) {
        case 0 ... 9: return (char) value + 48;
        case 10: return 'A';
        case 11: return 'B';
        case 12: return 'C';
        case 13: return 'D';
        case 14: return 'E';
        case 15: return 'F';
        default:
            sendERROR("Some bad value was passed to function toHexRep.\n");
            break;
    }

    //End of function return
    return '\0';
}


//Run a switch on the passed character to determine which integer value to return
int hexToDecimal(char hex)
{
    switch(hex) {
        case 48 ... 57: return (int) hex - 48;
        case 'A': return 10;
        case 'B': return 11;
        case 'C': return 12;
        case 'D': return 13;
        case 'E': return 14;
        case 'F': return 15;
        default:
            sendERROR("Some bad value was passed to function hexToDecimal.\n");
            break;
    }

    //End of function return
    return 999;
}



/*=============================================================================
| I Fletcher Morton (fl826999) affirm that this program is
| entirely my own work and that I have neither developed my code together with
| any another person, nor copied any code from any other person, nor permitted
| my code to be copied or otherwise used by any other person, nor have I
| copied, modified, or otherwise used programs created by others. I acknowledge
| that any violation of the above terms will be treated as academic dishonesty.
+=============================================================================*/
