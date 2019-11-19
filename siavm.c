#include <stdio.h>
#include <stdlib.h>


// Prototype for test functions
void testLoad();
void testFetch();
void testDecode();

/*
    4 Byte Instructions To Check For:
        branchifequal => opcode 10(0xA)
        branchifless => opcode 11(0xB)
        jump => opcode 12(0xC)
        call => opcode 13(0xD)
*/

// GLOBAL VARIABLES
// For memory
unsigned char memory[1024];
int byteCount = 0;
int currentInstructionSize = 0;
int currentByte = 0;

// Registers - All init to Zero
// Registers 0-14 general purpose
// Register 15 stack register
const int NUMBER_OF_REGISTERS = 16;
const int STACK = 15;
int Registers[16] = { 0 };

// Program counter
int PC = 0;

// Result
int Result = 0;

// OP1 & OP2
int OP1, OP2;

// Current Instruction
// Maxium 4 bytes per instruction
unsigned char currentInstruction[4];

// End of program flag
int END_OF_PROGRAM = 0;


// Takes in a file name 
// Read the file into vm's memory
// .bin file from assembler
void load(char* filename)
{
    // Tries to open file
    FILE *file = fopen(filename, "rb");

    // If not throw error
    if (file == NULL) 
    {
        perror("File specified was not successfully opened.");
        exit(-1);
    }

    // Gets file's contents one byte at a time and stores it in global memory
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
    // Gets the current byte of memory that should be fetched
    // Then updates the program counter by one
    currentByte = PC * 2;

    // Determines the opcode for the current instruction to grab the right amount of bytes from memory
    int instruction;

    // Checks to see if it is the end of the instructions
    if (!(instruction = memory[currentByte] >> 4)) 
    {
        END_OF_PROGRAM = 1;
        return;
    }
    
    // Determines the opcode for the current instruction to grab the right amount of bytes from memory

    // If the opcode is 10 || 11 || 12 || 13 set the current instruction size to 4 bytes
    // Else set it to 2 bytes
    switch (instruction)
    {
        case 10:
        case 11:
        case 12:
        case 13: currentInstructionSize = 4;
            PC += 2;
            break;
        default: currentInstructionSize = 2;
            PC++;
            break;
    }

    // Goes through and set the currentInstruction array to the bytes need to carry out instruction
    int count;
    for (count = 0; count < currentInstructionSize; count++)
    {
        currentInstruction[count] = memory[currentByte + count];
    }
}

// Read only from the array of bytes from fetch
// Populate the OP1 and OP2 registers if appropriate
// Or fetch additional memory needed to complete
void decode()
{
    // Gets whatever is after the opcode
    OP1 = (currentInstruction[0] >> 4 << 4) ^ (currentInstruction[0]);

    // Gets the second 8 bits
    // Depending this will be the top byte
    OP2 = currentInstruction[1];
}

// Switch statment that 'does the work'
// Stores the work into result
void execute()
{
    int count;

    switch(currentInstruction[0] >> 4) 
    {
        case 0: exit(0);
            break;
        // Add
        case 1: 
            Result = Registers[OP1] + Registers[OP2 >> 4];
            break;
        // And
        case 2:
            Result = Registers[OP1] && Registers[OP2 >> 4];
            break;
        // Divide
        case 3:
            Result = Registers[OP1] / Registers[OP2 >> 4];
            break;
        // Multiply
        case 4:
            Result = Registers[OP1] * Registers[OP2 >> 4];
            break;
        // Subtract
        case 5:
            Result = Registers[OP1] - Registers[OP2 >> 4];
            break;
        // Or
        case 6:
            Result = Registers[OP1] || Registers[OP2 >> 4];
            break;
        // Pop | Push | Return
        case 7:
            switch (OP2) 
            {
                case 0: 
                    
                    break;
                case 1: 
                    break;
                case 2:
                    break;
            }
            break;
        // Interrupt
        case 8:
            for (count = 0; count < NUMBER_OF_REGISTERS; count++) 
            {
                printf("R%d: %d\n", count, Registers[count]);
            }
            break;
        // Addimmediate
        case 9:
            Result = Registers[OP1] + OP2;
            break;
        // Branchifequal
        case 10:
            if (Registers[OP1] == Registers[OP2 >> 4])
            {
                int offset = (OP2 << 4 >> 4) + (currentInstruction[2]) + (currentInstruction[3]);
                PC = PC + (2 * offset);
            }
            break;
        // Branchifless
        case 11:
            if (Registers[OP1] > Registers[OP2 >> 4])
            {
                int offset = (OP2 << 4 >> 4) + (currentInstruction[2]) + (currentInstruction[3]);
                PC = PC + (2 * offset);
            }
            break;
        // Jump
        case 12:
            // Result is the 2-4 byte of the line times 2
            Result = (((OP1 << 28) & (OP2 << 24) & (currentInstruction[2] << 16) & currentInstruction[3]) * 2) + PC;
            break;
        // Call
        case 13:
            Result = (((OP1 << 28) & (OP2 << 24) & (currentInstruction[2] << 16) & currentInstruction[3]) * 2) + PC;
            Registers[STACK] = PC;
            break;
        // Load
        case 14:
            break;
        // Store
        case 15:
            break;
        default: perror("Opcode wasn't determined");
            break;

    }

}

// Write result into final resigster or memory address
void store()
{
    int resultRegister;

    switch(currentInstruction[0] >> 4)
    {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6: 
            // Gets register number from last 4 bits of OP2
            resultRegister = OP2 << 4 >> 4;
            Registers[resultRegister] = Result;
            PC += 2;
            break;
        case 7:
            break;
        case 8:
            END_OF_PROGRAM = 1;
            break;
        case 9:
            Registers[OP1] = Result;
            PC += 2;
            break;
        case 12:
        case 13:
            PC = Result;
    }
}

int main(int argc, char *argv[])
{
    // Check if file was specified in arguments
    // if so try loading file in load() function
    if (argv[1] == NULL) 
        perror("No input binary file specified. Please specify a file after siavm command");

    while (END_OF_PROGRAM == 0) 
    {

        load(argv[1]);

        fetch();

        decode();

        execute();
        printf("Result: %d\n", Result);

        store();
        printf("R%d: %d\n", OP1, Registers[OP1]);
    }
    
    
    return 1;
}

// ** Test Function Section ** //
// This is just for debugging/testing purposes
// Should be deleted for final submission
void testLoad()
{
    printf("Testing Load...\n\t=>");
    int count;
    for (count = 0; count < byteCount; count++)
    {
        printf("%x", memory[count]);

        if (count == (byteCount - 1)) printf("\n");
        else if (((count + 1) % 2) == 0) printf("\n\t=>");
    }
}

void testFetch()
{
    printf("Testing Fetch...\n\t=>");
    int count;
    for (count = 0; count < currentInstructionSize; count++) 
    {
        printf("%x", currentInstruction[count]);
    }
    printf("\n");
}

void testDecode()
{
    printf("Testing Decode...\n\t");
    printf("OP1: %x\n\tOP2: %x\n", OP1, OP2);
}