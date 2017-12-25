#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* 
The goal of this project was to write a program that translates 
logical address to physical address for a virtual address space 
of 2^16 (65,536) bytes. We read in the file containing logical 
addresses, using a TLB (translation look-aside buffer) as well 
as a page table, and translated each logical address to its 
corresponding physical address and output the value of the byte 
stored at the translated physical address. Then print out these 
values and ratios (page fault rate and TLB rate). 

To run: 
gcc -o VirtualMemory VirtualMemory.c
./VirtualMemory addresses.txt
*/

FILE *backStore;
FILE *addressFile;

#define LINELENGTH 10

#define PAGESIZE 256
int pageTable[PAGESIZE]; 
int pageFrame[PAGESIZE];

#define TLB_LENGTH 16
int TLBPage[TLB_LENGTH];
int TLBFrame[TLB_LENGTH];
int TLBNum = 0;
int TLBCounter = 0;

#define FRAMELENGTH 256
char readBacker[FRAMELENGTH];

#define physicalMemoryBytes 65536
int physicalMemory[physicalMemoryBytes];
int pageFault = 0;


/* Initialize array */
void initializeInfo(int *arr, int n) {

	int i = 0;

	for(i = 0; i < n; i++) {
		arr[i] = -1;
	}
}

/* Receives the page and reads from the BACK_STORE file and into the 
readBacker array. We then get the available frame and go through
the entire page size (256) and insert the info into the physical
memory array. Next we insert the frame into the page table and
increase the page faults. Finally we return the frame we used. */
int readBackStore(int page) {

	int i = 0, j = 0, availableFrame = 0, startFrameIndex = 0;

	/* SEEK_SET is in fseek() - it seeks from the beginning of the file */
	if(fseek(backStore, page * PAGESIZE, SEEK_SET) != 0) {
		printf("ERROR\n");
	}

	if(fread(readBacker, sizeof(signed char), PAGESIZE, backStore) == 0) {
		printf("ERROR\n");
	}

	/* Get available frame by looking for unused index in pageFrame */
	for(i =0; i < PAGESIZE; i++) {

		if(pageFrame[i] == -1) {

			pageFrame[i] = 0;
			availableFrame = i;
			break;
		}

	}

	/* Start at specific index for each frame */
	startFrameIndex = PAGESIZE * availableFrame;

	for(j = 0; j < PAGESIZE; j++) {

		physicalMemory[startFrameIndex] = readBacker[j];
		startFrameIndex++;

	}

	pageTable[page] = availableFrame;
	pageFault++;
	
	return availableFrame;

}

/* Gets the page and the offset of the logical address, checks if it's 
in the TLB page, if it is, save that frame. If not, then either reads 
the page from the page frame and saves the available frame to be used
to get info from the backstore into the physical memory array (this is
a page fault). Then the info is inserted into the TLB page and TLB frame.
We then return the physical memory address. */
int changeAddress(int logAddress) {


	int page = 0, i = 0, frameNum = -1, offset = 0;
	double origPage, decPage, intPage, offsetDub = 0.0;

	page = logAddress/PAGESIZE;

	origPage = (double)logAddress/PAGESIZE;
	decPage = modf(origPage, &intPage);

	offsetDub = decPage * PAGESIZE;
	offset = (int)offsetDub;


	/* check if page is in TLB frame */
	for(i = 0; i< TLB_LENGTH; i++) {

		if(TLBPage[i] == page) {

			frameNum = TLBFrame[i];
			TLBNum++;
		}

	}

	/* if page was not in TLB, read from BACK_STORE, or
	get page from pageTable */
	if(frameNum == -1) {

		/* if not in either, page fault */
		if(pageTable[page] == -1) {

			frameNum = readBackStore(page);

		} else {
			/* if not in TLB frame, get from pageTable */
			frameNum = pageTable[page];
		}

		TLBPage[TLBCounter%TLB_LENGTH] = page;
		TLBFrame[TLBCounter%TLB_LENGTH] = frameNum;
		TLBCounter++;

	}

	return (frameNum * PAGESIZE) + offset;	
}


int main(int argc, char *argv[]) {

	int translations = 0, logAddress = 0, address = 0;
	char line[LINELENGTH];

	if(argc != 2) {
		printf("Please enter two arguements.\nEx: ./file addresses.txt\n");
	}

	/* Open Files */
	backStore = fopen("BACKING_STORE.bin", "r");

	if(backStore == NULL) {

		printf("1 Null\n");
		return -1;
	}

	addressFile = fopen(argv[1], "r");

	if(addressFile == NULL) {

		printf("2 Null\n");
		return -1;
	}

	/* Initialize arrays */
	initializeInfo(pageTable, PAGESIZE);
	initializeInfo(pageFrame, PAGESIZE);
	initializeInfo(TLBPage, TLB_LENGTH);
	initializeInfo(TLBFrame, TLB_LENGTH);
	
	/* Go through each line of address file and pass logical address
	to Change address, which will translate the ino to a physical address */
	while(fgets(line, LINELENGTH, addressFile) != NULL) {

		logAddress = atoi(line);
		address = changeAddress(logAddress);

		printf("Logical Address: %d, Physical Memory: %d, Value: %d\n", logAddress, address, physicalMemory[address]);
		translations++;

	}

	/* Print out results */
	printf("\n*** Final Info ***\n");
	printf("Number of translations: %d\n", translations);
	printf("Number of Page Faults: %d\n", pageFault);
	printf("Page Fault Rate: %f\n",(float)pageFault/(float)translations);
	printf("Number of TLB Hits: %d\n", TLBNum);
	printf("TLB Rate: %f\n", (float)TLBNum/(float)translations);

	/* Close files */
	fclose(addressFile);
	fclose(backStore);

	return 0;
}

