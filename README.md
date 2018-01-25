# VirtualMemoryManager
The goal of this project was to write a program that translates 
logical address to physical address for a virtual address space 
of 2^16 (65,536) bytes. We read in the file containing logical 
addresses, using a TLB (translation look-aside buffer) as well 
as a page table, and translated each logical address to its 
corresponding physical address and output the value of the byte 
stored at the translated physical address. Then print out these 
values and ratios (page fault rate and TLB rate). 

![alt text](https://github.com/BrentLeeSF/VirtualMemoryManager/blob/master/VirtMem.png)

