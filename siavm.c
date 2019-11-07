#include <stdio.h>

// Costant Declarations
unsigned char memory[1024];
int byteCount = 0;

// Takes in a file name 
// Read the file into vm's memory
// .bin file from assembler
void load(char* filename)
{
    FILE *file = fopen(filename, "rb");

    int character = getc(file);

    while(character != EOF)
    {
        memory[byteCount] = character;

        character = getc(file);

        byteCount++;
    }

    fclose(file);
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
    if (argv[1] == NULL) 
        perror("No input binary file specified. Please specify a file after siavm command");
    else 
        load(argv[1]);

    int count;
    for (count = 0; count < byteCount; count++)
    {
        printf("%x", memory[count]);

        if (((count + 1) % 2) == 0) printf("\n");
    }
    
    return 1;
}