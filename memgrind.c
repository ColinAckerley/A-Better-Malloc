#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include "memgrind.h"
#include "mymalloc.h"


int main(int argc, char **argv)
{
	//definitions
	int i = 0;
	char *charPtr;
	struct timeval tv;
	double startTime, endTime, timeDiff;
	startTime = getTime(tv);
	char *arrPtr[150];
	srand(time(0));
	int totNumMalloc, totNumFree, currIndex;
	char *charArr[150];
	int randMallocSize, maxMemSize, currMemSize;
	char *charPtrs[150];
	int sizeOfMalloc[150];
	double caseA[100], caseB[100], caseC[100], caseD[100], caseE[100], caseF[100];

	int wlIter;
	wlIter = 0;
	for(wlIter = 0; wlIter<100; wlIter++){
	
	//Case A, malloc() 1 byte and immediately free it
	startTime = getTime(tv);
	for(i=0; i<150; i++){
		charPtr = malloc(sizeof(char));
		free(charPtr);
	}
	endTime = getTime(tv);
	timeDiff = endTime - startTime;
	if(timeDiff < 0){
		wlIter--;
		continue;
	}
	caseA[wlIter] = timeDiff;
	
	//Case B, malloc() 1 byte, store the pointer in an array
	startTime = getTime(tv);
	for(i=0; i<150; i++){
	charPtr = (char*)malloc(sizeof(char));
	arrPtr[i] = charPtr;
	}
	for(i=0; i<150; i++){
		free(arrPtr[i]);
	}
	endTime = getTime(tv);
	timeDiff = endTime - startTime;
	if(timeDiff < 0){
		wlIter--;
		continue;
	}
	caseB[wlIter] = timeDiff;	
	
	//Case C, Randomly choose between a 1 byte malloc() or free()ing a 1 byte pointer 
	totNumFree = 0;
	totNumMalloc = 0;
	currIndex = 0;
	startTime = getTime(tv);
	for(i=0; totNumFree<150;){
		if(rand()%2){
			if(totNumMalloc >= 150){
				continue;
			}
			else{
				charArr[currIndex] = (char*)malloc(sizeof(char));
				totNumMalloc++;
				currIndex++;
			}
		}
		else{
			if(totNumFree >= totNumMalloc){
				continue;
			}
			else{
				currIndex--;
				free(charArr[currIndex]);
				totNumFree++;
			}
		}
	}
	endTime = getTime(tv);
	timeDiff = endTime - startTime;
	if(timeDiff < 0){
		wlIter--;
		continue;
	}
	caseC[wlIter] = timeDiff;	
	
	//Case D, Randomly choose between a randomly-sized malloc() or free()ing a pointer
	totNumFree = 0;
	totNumMalloc = 0;
	currIndex = 0;
	currMemSize = 0;
	maxMemSize = 5000;
	startTime = getTime(tv);
	for(i=0; totNumFree<150;){
		if(rand()%2){
			if(currMemSize >= maxMemSize || totNumMalloc >= 150){
				continue;
			}
			else{
				randMallocSize = rand()%64+1;
				if(currMemSize+randMallocSize > maxMemSize){
					continue;
				}
				else{
					charPtrs[currIndex] = (char*)malloc(randMallocSize);
					sizeOfMalloc[currIndex] = randMallocSize;
					totNumMalloc++;
					currIndex++;
					currMemSize += randMallocSize;
				}
			}
		}
		else{
			if(totNumFree >= totNumMalloc){
				continue;
			}
			else{
				currIndex--;
				currMemSize -= sizeOfMalloc[currIndex];
				free(charPtrs[currIndex]);
				totNumFree++;
			}
		}	
	}
	endTime = getTime(tv);
	timeDiff = endTime - startTime;
	if(timeDiff < 0){
		wlIter--;
		continue;
	}
	caseD[wlIter] = timeDiff;
	
	//Case E, malloc max amount of memory (4998+2 bytes), then immediately free 150 times
	
	startTime = getTime(tv);
	
	for(i=0; i<150; i++){
	charPtr = (char*)malloc(4998*sizeof(char)); //4998 + 2bytes of metadata = 5000bytes
	free(charPtr);
	}
	
	endTime = getTime(tv);
	timeDiff = endTime - startTime;
	if(timeDiff < 0){
		wlIter--;
		continue;
	}
	caseE[wlIter] = timeDiff;
	
	
	//Case F, mallocs 4 times of 1000 bytes each, then frees each one, 
	//then mallocs a 2000 and 4000 byte block to test merging, does this 150 times
	char *arr1kbytes[1250];
	startTime = getTime(tv);
	int j = 0;
	
	for(j = 0; j<150; j++){
		for(i=0; i<4; i++){
			arr1kbytes[i] = (char*)malloc(1000);
		}
		for(i=0; i<4; i++){
			free(arr1kbytes[i]);
		}
		charPtr = (char*)malloc(2000);
		free(charPtr);
		charPtr = (char*)malloc(4000);
		free(charPtr);
	}
	
	endTime = getTime(tv);
	timeDiff = endTime - startTime;
	if(timeDiff < 0){
		wlIter--;
		continue;
	}
	caseF[wlIter] = timeDiff;
	
	
	}//end workLoadIters
	/*
	printf("caseA:\n");
	printArr(caseA);
	printf("caseB:\n");
	printArr(caseB);
	printf("caseC:\n");
	printArr(caseC);
	printf("caseD:\n");
	printArr(caseD);
	printf("caseE:\n");
	printArr(caseE);
	printf("caseF:\n");
	printArr(caseF);
	*/
	printf("Mean runtime for caseA: %f\n", calcMean(caseA));
	printf("Mean runtime for caseB: %f\n", calcMean(caseB));
	printf("Mean runtime for caseC: %f\n", calcMean(caseC));
	printf("Mean runtime for caseD: %f\n", calcMean(caseD));
	printf("Mean runtime for caseE: %f\n", calcMean(caseE));
	printf("Mean runtime for caseF: %f\n", calcMean(caseF));
	
    return 0;
}



double getTime(struct timeval time){
	gettimeofday(&time, NULL);
	return ((double)time.tv_usec/1000000);
}

double calcMean(double *arrTimes){
	int i = 0;
	double sum = 0;
	for(i=0; i<100; i++){
		sum+=arrTimes[i];
	}
	return (sum/100);
}

void printArr(double *arrTimes){
	int i = 0;
	for(i=0; i<100; i++){
		//printf("iter %d = %f\n", i, arrTimes[i]);
	}
}
