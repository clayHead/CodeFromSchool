//main.c program to translate the virtual addresses from the virtual.txt
//into the physical frame address. Project 3 - CSci 4061

#include <stdio.h>
#include <stdlib.h>
#include "vmemory.h"

#define INPUT_FILE "../bin/virtual.txt"

int main(int argc, char* argv[])
{
	if (argc > 2) {
		printf("Too many arguments, enter up to one argument\n");
		exit(-1);
	}

	int policy = (argc == 2) ? 1:0;
	initialize_vmanager(policy);

  FILE *f;
  unsigned int num[1000];
  int frame;
  int offset;
  f = fopen(INPUT_FILE, "r");
  if(f == NULL) {
    printf("File %s could not be opened.\n", INPUT_FILE);
    exit(1);
  }
  int i = 0;
  while(fscanf(f, "%x", &num[i]) == 1) {
    i++;
  }
  fclose(f);

  for(int j = 0; j < i; j++) {
    frame = get_tlb_entry(num[j]);  // try to get the page from the tlb
    if(frame == -1) {
      frame = fetch_physical_frame(num[j]);  // get frame if not from tlb
      populate_tlb(num[j], frame);  // put frame in the tlb
    }
    offset = num[j] & 0xFFF;  // get offset
    print_physical_address(frame, offset);  // output to screen
    if(j % 10 == 0) {
      print_tlb();
      printf("Hit ratio: %f\n", get_hit_ratio());
    }
  }

	//Free the page table
	free_resources();
	return 0;
}
