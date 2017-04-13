#include <stdio.h>
#include <stdlib.h>
#include <math.h>

FILE *addressFile;
#define LINELENGTH 10
char line[LINELENGTH];
int logAddress = 0;
#define pageSize 256
int pageTable[pageSize]; 
int firstEmptyPage;


void changeAddress(int logAddress);

int main(int argc, char *argv[]) {

	if(argc != 2) {
		printf("Please enter two arguements.\nEx: ./file addresses.txt\n");
	}

	addressFile = fopen(argv[1], "r");
	if(addressFile != NULL) {
		printf("Not null\n");
	}
	int i;
	for(i = 0; i < pageSize; i++) {
		pageTable[i] = -1;
	}
	while(fgets(line, LINELENGTH, addressFile) != NULL) {
		logAddress = atoi(line);
		printf("%d\n",logAddress);
		changeAddress(logAddress);
	}

	return 0;
}

void changeAddress(int logAddress) {

	double page, offset, intPage;
	page = (double)logAddress/pageSize;
	offset = modf(page, &intPage);
	offset *= (double)pageSize;

	int newPage, newFractPage;
	newPage = (int)intPage;
	newFractPage = (int)offset;
	printf("Int: %d, Fract: %d\n", newPage, newFractPage);

	int frameNum = pageTable[newPage];
	if(frameNum == -1) {
		pageTable[newPage] = firstEmptyPage;
		printf("PageTable: %d, NewPage: %d\n",pageTable[newPage], firstEmptyPage);
		firstEmptyPage++;
	}
	else {
		printf("Not -1: %d\n",frameNum);
	}
}

