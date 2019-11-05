#include <stdio.h>

// Costant Declarations
unsigned char memory[1024];

// Takes in a file name 
// Read the file into vm's memory
// .bin file from assembler
void load(char* filename)
{
    FILE *file = fopen(filename, "rb");

    int character = getc(file);
    int count = 1;

    while(character != EOF)
    {
        if ((count % 8) == 0) 
        {
            memory[0] = character;
            return;
        }

        character += getc(file);
        count++;
    }
}

// Read instruction bytes from memory
void fetch()
{

}

// Read only from the array of bytes from fetch
// Populate the OP1 and Op2 registers if appropriate
// Or fetch additional memory needed to complete
void decode()
{

}

// Switch statment that 'does the work'
// Stores the work into result
void execute()
{

}

// Write result into final resigster or memory address
void store()
{

}

int main(int argc, char *argv[])
{
    load("test.bin");

    printf("%x", memory[0]);
    
    return 1;
}