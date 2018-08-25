#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"
#define MEMSIZE 5000
char mem[MEMSIZE];
int isFirstAlloc = 1;
char *ptr;
/*
int main(int argc, char **argv)
{
    mymalloc(500, NULL, 100);
    mymalloc(200, NULL, 100);
}
*/
void* mymalloc(unsigned int size, char* filename, int line)
{
	printf("NEW MALLOC------------------\n");
    if(isFirstAlloc == 1) //If we are allocating for the first time, we need to setup an intial block of size MEMSIZE - 2
    {
        setSize(0, MEMSIZE - 2);
        isFirstAlloc = 0;
    }
    if(size == 0) //Check for some simple errors here
    {
        printf("ERROR: You cannot allocate memory with size 0\n" "Filename: %s\n Line: %d\n",filename, line);
        return NULL;
    }
    else if(size >= MEMSIZE)
    {
        printf("Error first\n");
        printf("ERROR: Not enough memory available for allocation\n" "Filename: %s\n Line: %d\n",filename, line);
        return NULL;
    }
   else if(getAvailBlock(size) >= MEMSIZE - 2)
    {
    	//printf("ELSEIF SIZE = %d\n", getAvailBlock(size));
    	printf("Error second\n");
        printf("ERROR: Not enough memory available for allocation\n" "Filename: %s\n Line: %d\n",filename, line);
        return NULL;
    }
    unsigned int block = getAvailBlock(size);
    if(block > 32768)
    {
        printf("ERROR: Not enough memory available for allocation\n" "Filename: %s\n Line: %d\n",filename, line);
        return NULL;
    }
    unsigned int remainingSize = 0;
    unsigned int initialSize = getSize(block); //Store the initial size in case we have any left over space
    if(initialSize - size - 2 > 0)
    {
        remainingSize = initialSize - size - 2; //Stores the remaining space if there is any left
    }
    setSize(block, size); //Sets the size of our block
    mem[block] |= 10000000; //Marks the block we have as allocated
    if(remainingSize > 0)
    {
        unsigned int leftoverBlockLocation = block + size + 2;
        setSize(leftoverBlockLocation, remainingSize);
    }
    return &mem[block + 2]; //Return the location of the actual memory (2 added to skip over metadata)
}
void myfree(void* userPointer, char* filename, int line)
{
    if(userPointer == NULL) //Cannot free a null pointer
    {
        printf("ERROR: Cannot free a null pointer\n" "Filename: %s\n Line: %d\n",filename, line);
        return;
    }
    int curSpot = 2;
    int checkIfValid = 0; //See if the pointer exists or not
    while(curSpot < MEMSIZE)
    {
        if(userPointer == &mem[curSpot])
        {
            checkIfValid = 1; //The pointer they gave us actually exists
            break;
        }
        else
            curSpot = curSpot + 2 + getSize(curSpot); //Otherwise, skip ahead by a block
    }
    if(mem[curSpot - 2] >> 7 == 0) //If the pointer they gave is already deallocated, this means it has already been freed
    {
        printf("ERROR: Pointer already freed\n" "Filename: %s\n Line: %d\n",filename, line);
        return;
    }
    else if(checkIfValid == 0) //If they didn't give us a valid pointer
    {
        printf("ERROR: Pointer never allocated\n" "Filename: %s\n Line: %d\n",filename, line);
        return;
    }
    mem[curSpot - 2] = mem[curSpot - 2] ^ 10000000; //XOR in order to clear the most significant bit while preserving the size
    if(mem[curSpot + getSize(curSpot - 2)] >> 7 == 0) //Checks if the next available block is free
    {
        int nextBlock = curSpot + getSize(curSpot - 2); //Finds the location of the next block
        int sizeOfNextBlock = getSize(nextBlock);
        setSize(curSpot - 2, sizeOfNextBlock + getSize(curSpot - 2)); //Combines the current block and the next block
        setSize(nextBlock, 0); //Sets the size of the block to 0 just to be safe
    }
    int prev = -1;
    int i;
    for(i = 0; i < curSpot - 2; i += getSize(i)) //Check if the previous block is free
    {
        if(getSize(i) + i + 2 == curSpot) //i will always be a metadata block
        {
            prev = i;
            break;
        }
    }
    if(prev != -1) //If we found  a previous block
    {
        if((mem[prev] >> 7) == 0) //If the previous block is free
        {
            setSize(prev, getSize(prev) + getSize(curSpot - 2)); //Combine the size of the two blocks
            setSize(curSpot - 2, 0); //Reset the size of the old block to be safe
        }
    }
}
void setSize(unsigned int index, unsigned int size)
{
	
    unsigned int firstBits = size >> 8; //Only get the first 8 bits
    unsigned int lastBits = (((1<<8)-1)&size)&0xFF;
    //unsigned int lastBits = size & 0xFF; //Get the last 8 bits
    mem[index] = firstBits; //Set our first meta byte to our first 8 bits (7 since the allocation bit doesnt count)
    mem[index + 1] = lastBits; //Set our other meta byte to our last 8 bits
    printf("memindex = %u\n", mem[index]);
    printf("memindex+1 = %u\n", (((1<<8)-1)&mem[index+1]));
    printf("LASTBITS = %u\n", lastBits);
}
unsigned int getSize(unsigned int index)
{	
	unsigned int resFirst, resLast;
	resFirst = (((1<<7)-1)&mem[index]);
	resLast = (((1<<8)-1)&mem[index+1]);
    printf("RESULT = %u\n", resFirst*256+resLast);
    return resFirst*256+resLast;    
}
unsigned int getAvailBlock(unsigned int size)
{
    unsigned int curBlock = 0;
    while(curBlock < MEMSIZE)
    {
        if((mem[curBlock] >> 7 == 0) && (size <= getSize(curBlock))) //The block is large enough and it is not currently allocated
            break;
        else
            curBlock = curBlock + 2 + getSize(curBlock); //Otherwise the block is alloacted, so skip that many bytes ahead
    }
    if(curBlock + 2 >= MEMSIZE) //If the block we found is beyond our MEMSIZE, return an invalid number
    {
        return 99999999;
    }
    else
        return curBlock; //Return the current block since it is not allocated and it is large enough
}
