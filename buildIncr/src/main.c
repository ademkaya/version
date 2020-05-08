// buildIncr.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define _CRT_SECURE_NO_WARNINGS
// #define BUILD   13

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
/*

	argc (ARGument Count) is int and stores number of command-line arguments passed by the user including the name of the program. 
	So if we pass a value to a program, value of argc would be 2 (one for argument and one for program name)
	The value of argc should be non negative.

	argv(ARGument Vector) is array of character pointers listing all the arguments.
	If argc is greater than zero,the array elements from argv[0] to argv[argc-1] will contain pointers to strings.
	Argv[0] is the name of the program , After that till argv[argc-1] every element is command -line arguments.

*/

#define DEBUG

// Dynamics
FILE* fptr = NULL;
FILE* ftempptr = NULL;
static char* fileContent=NULL;
static char* tempBuffer = NULL;
static char* filePathNext = NULL;
static char* filePath = NULL;

// func prototypes
void	 arrayShiftLeft(char* StartPoint, uint16_t arraySize, uint16_t shiftAmount);
bool	 StrComp(const char* ptr1,const char* ptr2, int length);
bool	 SearchInArray(char* bigger, int bLength, const char* smaller, int sLength, uint32_t* spotPoint);
uint32_t FindNextNewLine(char* strArray);
void	 arrClear(void* ptr, uint32_t _size);
uint32_t getfilecharacterCount(FILE** ifptr);
uint32_t getcharacterCount(char* array);
uint32_t getDecimalCounts(uint32_t number);
void	 editArray(char* oldArray, uint32_t startPoint, char* newArray, uint32_t newArrayLength);
void	 clearBuffers();

#define  size 14
static const char strConst[] = "#define BUILD ";
char tempArray[size];

int main(int argc, char** argv)
{	
	bool getValue = false;
	filePath = argv[1];	
	uint32_t fsz, fcntr = 0;

        // if the is no filepath enterence return
	if (!filePath) {
		printf("no filepath\n");
                printf("USAGE >>  X\\:build_ncr.exe FILEPATH  \n");
		exit(0);
	}               
        
	uint32_t pathLength = getcharacterCount(filePath);

	// dosya adresi kopyalan�p yeni dosya bu adrese yazd�r�lacak!!!!
	// copy the raw data
	filePathNext = realloc(filePathNext, sizeof(char) * (pathLength + 100));
	if (!filePathNext) {
		clearBuffers();
		printf("allocation error");
		exit(0);
	}
	strcpy(filePathNext, filePath);

	// concatenate the file path
	strcat(filePath, "\\version.h");

	// open the file in read write mode	
	fptr = fopen(filePath, "r");
		if (!fptr) {
			printf("unable to open the file path");
			exit(0);
		}

	//get the size of the file
	fseek(fptr, 0L, SEEK_END);
		fsz = ftell(fptr);
	rewind(fptr);

	// then initialize and array to hold the content of the file
	fileContent = (char*)realloc(fileContent, sizeof(char) * fsz);
	if (!fileContent) {
		clearBuffers();
		printf("allocation error");
		exit(0);
	}
		
	// copy the file into the buffer:
	uint32_t retVal = 0;
	retVal = fread(fileContent, 1, fsz, fptr);

	// then search the array in order to find the check point
	uint32_t spottedIndex = 0;
	bool result = SearchInArray(fileContent, fsz, &strConst[0], size,&spottedIndex);

	// if nothing if spotted
	if (!result){
		// free(fileContent);
		clearBuffers();
		printf("nothing spotted");
		fclose(fptr);
		exit(0);
	}
	else {
		uint32_t nextNewLine = 0;
		nextNewLine = FindNextNewLine(fileContent + spottedIndex);
		uint32_t build_number = atoi(fileContent + spottedIndex + size);	// #define DEBUG_BUILD <show here>
		printf("%d", build_number);

		// increase the number 1
		build_number += 1;

		// get the decimal length of the array
		uint32_t decimalcounts = getDecimalCounts(build_number);

		tempBuffer= realloc(tempBuffer, sizeof(char) * (decimalcounts+10));
		if (!tempBuffer) {
			clearBuffers();
			printf("allocation error");
			exit(0);
		}
		sprintf(tempBuffer, "%d", build_number);
		// then edit it back
		editArray(fileContent, spottedIndex+ size, tempBuffer, decimalcounts);
	}

	// So far everything is ok write the data into new file
	
	// concatenate the file path
	 strcat(filePathNext, "\\versiontemp.h");

	ftempptr = fopen(filePathNext, "w");
	if (!ftempptr) {
		printf("unable to open the temp file path");
		clearBuffers();
		exit(0);
	}
	fwrite(fileContent, sizeof(char), fsz, ftempptr);

	//close the both files
	fclose(fptr);
	fclose(ftempptr);

	// now updating is done, delete the old file
	bool status = remove(filePath);
	if (status) {
		printf("unable to remove the first copy\n");
	}
	//file copy is removed, rename the temp file 
	status = false;
	status = rename(filePathNext, filePath);
	if (status) {
		printf("unable to rename\n");
	}

	clearBuffers();

	return 0;
}

void clearBuffers() {
	static bool alreadyDone = false;

	if (alreadyDone)
		return;

	if (tempBuffer)
		free(tempBuffer);

	if (fileContent)
		free(fileContent);

	if (filePathNext)
		free(filePathNext);


	if (fptr)
		fclose(fptr);

	if (ftempptr)
		fclose(ftempptr);
	
	alreadyDone = true;
}

uint32_t getfilecharacterCount(FILE** ifptr) {
	uint32_t retVal = 0;
	char ch = 0;
	while ((ch = fgetc(*ifptr)) != EOF) {
		retVal++;
	}
}
uint32_t getcharacterCount(char* array) {
	uint32_t i = 0;
	while (*(array+i)) {
		i += 1;
	}
	return i;
}
void arrClear(void* ptr, uint32_t _size) {
	uint32_t __size = 0;
	for (__size = 0; __size < _size; __size++)
	{
		*((char*)ptr + __size) = 0;
	}
}

void arrayShiftRight(char* StartPoint, uint16_t arraySize, uint16_t shiftAmount) {
	uint32_t sAmount = 0;
	uint32_t aSize = 0;

	for (sAmount = 0; sAmount < shiftAmount; sAmount++) {
		for (aSize = 0; aSize < arraySize; aSize++) {
			*(StartPoint + (arraySize - 1) - aSize) = *(StartPoint + (arraySize - 1) - (aSize + 1));
		}
		*(StartPoint + 0) = 0x0000;
	}
}
void arrayShiftLeft(char* StartPoint, uint16_t arraySize, uint16_t shiftAmount) {
	uint32_t sAmount = 0;
	uint32_t aSize = 0;

	for (sAmount = 0; sAmount < shiftAmount; sAmount++) {
		for (aSize = 0; aSize < arraySize; aSize++) {
			*(StartPoint + aSize) = *(StartPoint + aSize + 1);
		}
	}
}
/* works for equal length */
bool StrComp(const char* ptr1,const char* ptr2, int length) {
	int i = 0;
	bool retVal = true;
	for (i = 0; i < length; i++) {
		if (*(ptr1 + i) != *(ptr2 + i)) {
			retVal = false;
			break;
		}
	}
	
	return retVal;
}
bool SearchInArray(char* bigger, int bLength,const char* smaller, int sLength,uint32_t* spotPoint) {
	int i = 0;
	bool retVal = false;
	for (i = 0; i < (bLength - sLength); i++) {
		*spotPoint = i;
		if (StrComp(bigger + i, smaller, sLength)) {
			retVal = true;
			break;
		}
	}

	if (!retVal)
		*spotPoint = 0;

	return retVal;
}
uint32_t FindNextNewLine(char* strArray) {
	uint32_t i = 0;
	bool spotted = false;
	while (strArray + i) {		
		if (*(strArray + i)=='\n') {
			spotted = true;
			break;
		}
		i += 1;
	}

	if (!spotted)
		i = 0;

	return i;
}
uint32_t getDecimalCounts(uint32_t number) {
	uint32_t n = number;
	uint32_t i = 0;

	while (n) {
		i += 1;
		n /= 10;
	}

	return i;
}

void editArray(char* oldArray, uint32_t startPoint, char* newArray, uint32_t newArrayLength) {
	uint32_t i = 0;
	for (i = 0; i < newArrayLength; i++) {
		*(oldArray + startPoint + i) = *(newArray + i);
	}
}














