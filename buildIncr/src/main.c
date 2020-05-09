// buildIncr.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define _CRT_SECURE_NO_WARNINGS

#include <time.h>
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

//  #define DEBUG

// Dynamics
FILE* fptr = NULL;
FILE* ftempptr = NULL;
static char* fileContent=NULL;
static char* tempBuffer = NULL;
static char* filePathNext = NULL;
static char* filePath = NULL;
    bool filePathAllocated=false;
static char* currentDate=NULL;


// func prototypes
uint32_t         copyFileIntoBuffer(FILE* ptr,char* buffer);
void             arrayShiftLeft(char* StartPoint, uint16_t arraySize, uint16_t shiftAmount);
bool             StrComp(const char* ptr1,const char* ptr2, int length);
bool             SearchInArray(char* bigger, int bLength, const char* smaller, int sLength, uint32_t* spotPoint);
uint32_t         FindNextNewLine(char* strArray);
// void             arrClear(void* ptr, uint32_t _size);
uint32_t         getfilecharacterCount(FILE** ifptr);
uint32_t         getcharacterCount(char* array);
uint32_t         getDecimalCounts(uint32_t number);
void             editArray(char* oldArray, uint32_t startPoint, char* newArray, uint32_t newArrayLength);
static void	 clearBuffers();
bool             returnCurrentTimeDate(char** c_time_string,bool addApostropfe);
void             strcpyWithOffset(char* destination, uint32_t destOffset, char* src, uint32_t length);

static const char  strMAJORConst[] = "#define MAJOR ";
static const char  strMINORConst[] = "#define MINOR ";
static const char  strRELEASEConst[] = "#define RELEASE ";
static const char  strBUILDConst[] = "#define BUILD ";
static const char  strLASTBUILDConst[] = "#define LASTBUILD ";

static const char* constPtrArray[]= {&strMAJORConst[0],&strMINORConst[0],&strRELEASEConst[0],&strBUILDConst[0],&strLASTBUILDConst[0]};
static uint8_t     constLength[] = {sizeof(strMAJORConst)-1,sizeof(strMINORConst)-1,sizeof(strRELEASEConst)-1,sizeof(strBUILDConst)-1,sizeof(strLASTBUILDConst)-1};
static uint32_t   versionValues[4]={0,0,0,0};


int main(int argc, char** argv)
{	 
    // buildincr.exe C:\Users\Adem\Desktop\New folder\version.h
    
    
    // file path empty space fix because of argument assignment
    if (argc>2){
        filePathAllocated = true;        
        filePath = realloc(filePath, sizeof(char) * 1024);        
        if (!filePath) {
                printf("allocation error");
		clearBuffers();		
		exit(EXIT_FAILURE);
	}else{
            memset(filePath,0,1024);            
        }
        uint8_t argi=0;
        for(argi=1;argi<argc;argi++){
            strcat(filePath,argv[argi]);
            if (argi!= argc-1)
                strcat(filePath," ");            
        }    
    }else{        
        filePath = argv[1];        
    }

    
    
    
        // if the is no filepath enterence return
	if (!filePath) {
		printf("no filepath\n");
                printf("USAGE >>  X\\:build_ncr.exe FILEPATH  \n");
		exit(EXIT_FAILURE);
	}               
       
        
        /*TIME*/
        currentDate=NULL;
        if (!returnCurrentTimeDate(&currentDate,true)){
            printf("Time Date error");
            exit(EXIT_FAILURE);
        }
           

        
        /*TEMP FILE PATH*/
	uint32_t pathLength = getcharacterCount(filePath);
	// copy the raw data
	filePathNext = realloc(filePathNext, sizeof(char) * (pathLength + 100));
	if (!filePathNext) {
		clearBuffers();
		printf("allocation error");
		exit(EXIT_FAILURE);
	}else{
            memset(filePathNext,0,(pathLength + 100));
        }        
	strcpy(filePathNext, filePath);
        
        
        

	// open the file in read write mode	
	fptr = fopen(filePath, "r");
		if (!fptr) {
			printf("unable to process the file path");
			exit(EXIT_FAILURE);
		}

	// then initialize and array to hold the content of the file
	fileContent = (char*)realloc(fileContent, sizeof(char) * 1024);
	if (!fileContent) {
		clearBuffers();
		printf("allocation error");
		exit(EXIT_FAILURE);
	}else{
            memset(fileContent,0,1024);
        }
		
	// copy the file into the buffer:
	uint32_t charCount = 0;
        charCount = copyFileIntoBuffer(fptr,fileContent);

        
        
        
        
        
//->>>>>>>        
        // cycle through the file to find major minor release and build values
        for(uint8_t _i=0;_i<5;_i++){
            
            // then search the array in order to find the check point
            uint32_t spottedIndex = 0;
            bool result = SearchInArray(fileContent, charCount, constPtrArray[_i], constLength[_i],&spottedIndex);  //control edilecek
            
            // if nothing if spotted
            if (!result){
                    // free(fileContent);
                    clearBuffers();
                    printf("nothing or less spotted");
                    fclose(fptr);
                    exit(EXIT_FAILURE);
            } else {
                if(_i==4){
                    
                    // date time spotted
                    editArray(fileContent, spottedIndex+ constLength[_i], currentDate,getcharacterCount(currentDate)); 
                }else{
                    
                    versionValues[_i] = atoi(fileContent + spottedIndex + constLength[_i]);
                    
                    if (_i==3){
                        
                        versionValues[_i]+=1;
                        
                        // get the decimal length of the NUMBER
                        uint8_t decimalcounts = getDecimalCounts(versionValues[_i]);
                        tempBuffer= realloc(tempBuffer, sizeof(char) * (decimalcounts+10));
                        if (!tempBuffer) {
                            clearBuffers();
                            printf("allocation error");
                            exit(EXIT_FAILURE);
                        }else{
                            memset(tempBuffer,0,(decimalcounts+10));
                        }
                        sprintf(tempBuffer, "%d", versionValues[_i]);    // CONVERT CHAR
                        // then edit it back ,
                        editArray(fileContent, spottedIndex+ constLength[_i], tempBuffer, decimalcounts);                                                
                    }         
                }
            }                                                
        }
 
        // print it out
        printf("\n\n\nv%d.%d.%d.%d\n%s\n\n\n\n",versionValues[0],versionValues[1],versionValues[2],versionValues[3],currentDate);
        
 //>>>>>>>>>       
        
        
        
	// concatenate the file path
	 strcat(filePathNext, "__");

	ftempptr = fopen(filePathNext, "w");
	if (!ftempptr) {
		printf("unable to open the temp file path");
		clearBuffers();
		exit(EXIT_FAILURE);
	}
//	fwrite(fileContent, sizeof(char), fsz, ftempptr);   removed
        fwrite(fileContent, sizeof(char), charCount, ftempptr);        

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
uint32_t copyFileIntoBuffer(FILE* ptr,char* buffer){    
    	uint32_t i=0;
	char ch = 0;
	while ((ch = fgetc(ptr)) != EOF) {
		*(buffer+i) = ch;
                i+=1;
	}
        *(buffer+i) = '\0';
    return i;
}
static void clearBuffers() {
	static bool alreadyDone = false;

	if (alreadyDone)
		return;

        if (currentDate){
            free(currentDate);
            currentDate = NULL;
        }
        
	if (tempBuffer){
		free(tempBuffer);
                tempBuffer=NULL;
        }

	if (fileContent){
		free(fileContent);
                fileContent = NULL;
        }
        
        if ((filePath)&&(filePathAllocated)){
		free(filePath);
                filePath = NULL;
        }
        
	if (filePathNext){
		free(filePathNext);
                filePathNext = NULL;
        }

	if (fptr){
		fclose(fptr);
                fptr = NULL;
        }
        
	if (ftempptr){
		fclose(ftempptr);
                ftempptr = NULL;
	}
        
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
		if (StrComp(bigger + i, smaller, sLength-1)) {
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
bool returnCurrentTimeDate(char** c_time_string,bool addApos){
    
    bool retVal=true;
    time_t current_time;
    char* tempHolder = NULL;

    if(*c_time_string==NULL){
        *c_time_string = realloc(*c_time_string,sizeof(char)*100);
        tempHolder = realloc(tempHolder, sizeof(char) * 100);
        if ((!*c_time_string)||(!tempHolder)){
            return false;
        }
    }
    memset(*c_time_string,0, 100);
    memset(tempHolder, 0, 100);
        
    /* Obtain current time. */
    current_time = time(NULL);

    if (current_time == ((time_t)-1))
    {
        return false;
    }

    if(addApos){
        /* Convert to local time format. */
        tempHolder = ctime(&current_time);
        int g = getcharacterCount(tempHolder)-1; // -1 for \n
        *c_time_string[0] = (char)'"';
        strcpyWithOffset(*c_time_string, 1, tempHolder, g);
        char a[2] = {'"',0};
        strcat(*c_time_string,(const char*)&a);

        free(tempHolder);
        tempHolder = NULL;

    }else{
        *c_time_string = ctime(&current_time);
    }


    if (*c_time_string == NULL)
    {
        return false;
    }

    return retVal;
}

void strcpyWithOffset(char* destination, uint32_t destOffset, char* src, uint32_t length) {
    uint32_t i = 0;
    for (i = 0; i < length; i++) {
        *(destination + destOffset + i) = *(src + i);
    }
}












