#include <stdio.h>
#include <stdlib.h>
#include <math.h>


FILE *addressFile;
#define LINELENGTH 10
char line[LINELENGTH];
int logAddress = 0;

#define PAGESIZE 256
int pageTable[PAGESIZE]; 
int pageFrame[PAGESIZE];
//int firstEmptyPage;

#define TLB_LENGTH 16
int TLBPage[TLB_LENGTH];
int TLBFrame[TLB_LENGTH];
int TLBNum = 0;
int TLBCounter = 0;

int availablePageNumber = 0;

FILE *backStore;

#define FRAMELENGTH 256
signed char readBacker[FRAMELENGTH];
#define FRAMESIZE 256
int physicalLocation[FRAMELENGTH][FRAMESIZE];
int frameOpen = 0;
int pageFault = 0;

int physicalAddress = 0;


void changeAddress(int logAddress);
void readBackStore(int page);


int main(int argc, char *argv[]) {

	if(argc != 2) {
		printf("Please enter two arguements.\nEx: ./file addresses.txt\n");
	}

	backStore = fopen("BACKING_STORE.bin", "rb");
	if(backStore == NULL) {
		printf("Null\n");
		return -1;
	}

	addressFile = fopen(argv[1], "r");
	if(addressFile == NULL) {
		printf("Null\n");
		return -1;
	}

	int i;
	for(i = 0; i < PAGESIZE; i++) {
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

	double origPage, offset, intPage;
	origPage = (double)logAddress/PAGESIZE;
	offset = modf(origPage, &intPage);
	offset *= (double)PAGESIZE;

	int page, newFractPage;
	page = (int)intPage;
	newFractPage = (int)offset;
	printf("Int: %d, Fract: %d\n", page, newFractPage);

	int frameNum = -1;

	int i;
	// check if in TLB frame
	for(i = 0; i< TLB_LENGTH; i++) {
		if(TLBPage[i] == page) {
			frameNum = TLBFrame[i];
			TLBNum++;
			break;
		}
	}

	if(frameNum == -1) {
		// if not in TLB frame, check in pageTable
		for(i = 0; i < availablePageNumber; i++) {
			if(pageTable[i] == page) {
				frameNum = pageFrame[i];
				break;
			}
		}
		// if not in either, page fault
		if(frameNum == -1) {
			readBackStore(page);
		}

		TLBPage[TLBCounter%TLB_LENGTH] = page;
		TLBFrame[TLBCounter%TLB_LENGTH] = frame;
		TLBCounter++;
	}

	physicalAddress = (frame * PAGESIZE) + offset;

	printf("Logical Address: %d, Physical memory: %d, Value: %d\n", logAddress, physicalAddress, physicalLocation[]);
	
}

void readBackStore(int page) {
	// SEEK_SET is in fseek() - it seeks from the beginning of the file
	if(fseek(backStore, page * FRAMELENGTH, SEEK_SET) != 0) {
		printf("ERROR\n");
	}

	int sizin = sizeof(signed char);
	if(fread(readBacker, sizin, FRAMELENGTH, backStore) == 0) {
		printf("ERROR\n");
	}

	int i;
	for(i =0; i < FRAMELENGTH; i++) {
		physicalLocation[frameOpen][i] = readBacker[i];
	}
	frameOpen++;


}


