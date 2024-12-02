//Implement the API, modeling the translation of virtual page address to a
//physical frame address. We assume a 32 bit virtual memory and physical memory.
//Access to the page table is only via the CR3 register.
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <math.h>
#include <fcntl.h>
#include "vmemory.h"
#define OUT_TLB "../bin/tlb_out.txt"

bool FIFO_policy = true;
int **cr3;


//
// More static functions can be implemented here
//

#define MID_MASK_10	0x3FF000
#define OFFSET_MSK 0xFFF
#define FRONT_MASKMID 0xFFFFF000
#define FRONT_MASK_10 0xFFC00000

int front = 0;
int mid = 0;
int offs = 0;

struct tlb {
  int data[8][2];
  int hits;
  int inquiries;
  int size;
};

int **cr3;
struct tlb my_tlb;


static void initialize_tlb() {
  my_tlb.size = 0;
  my_tlb.hits = 0;
  my_tlb.inquiries = 0;
  for(int i = 0; i < 8; i++){
    my_tlb.data[i][0] = -1;
    my_tlb.data[i][1] = -1;
  }
  return;
}

// The implementation of get_vpage_cr3 is provided in
// an object file, so no need to re-implement it
void initialize_vmanager(int policy)
{
	// Set LRU policy when passsed as a parameter
	if (policy)
		FIFO_policy = false;
	cr3 = get_vpage_cr3();
	//printf("cr3: %p\n", cr3);
  initialize_tlb();
}

//
// The implementation of following functions is required
//
int fetch_physical_frame(unsigned int v_addr)
{
  front = 0;
  mid = 0;
  offs = 0;

	int bytes[3];
	bytes[0] = (v_addr & FRONT_MASK_10) >> 22;
	bytes[1] = (v_addr & MID_MASK_10) >> 12;
	bytes[2] = (v_addr & OFFSET_MSK) >> 0;

	front = bytes[0];
	mid = bytes[1];
	offs = bytes[2];

	if(cr3[(int)front] == 0) {
		return -1;
	} else if(cr3[(int)front][(int)mid] == 0) {
		return -1;
	} else {
		return (int)cr3[(int)front][(int)mid];
	}
}


void print_physical_address(int frame, int offset)
{
	int results = 0;
	if(frame == -1) {
		printf("%d \n", -1);
	} else{
		results = (frame<<12) + offset;
		printf("0x%x \n", results);
	}
}

int get_tlb_entry(int n)
{
  /*if(my_tlb == NULL) {
    printf("\n");  // TODO REMOVING THIS BREAKS THE PAGE TABLE FREE
    initialize_tlb();
  }*/
  my_tlb.inquiries++;
  int vir_page_num = (n & 0xFFFFF000) >> 12;
  for(int i = 0; i < my_tlb.size; i++) {
    if(my_tlb.data[i][0] == vir_page_num) {
      my_tlb.hits++;
      return my_tlb.data[i][1];  // return frame base
    }
  }
  return -1; // not in the table
}

void populate_tlb(int v_addr, int p_addr)
{
  // not in the tlb
  if(my_tlb.size == 8) {
    // tlb is full, use FIFO policy
    for(int i = 0; i < 7; i++) {
      my_tlb.data[i][0] = my_tlb.data[i+1][0];
      my_tlb.data[i][1] = my_tlb.data[i+1][1];
    }
    my_tlb.size--;
  }
  int v = (v_addr & 0xFFFFF000) >> 12;
  my_tlb.data[my_tlb.size][0] = v;
  my_tlb.data[my_tlb.size][1] = p_addr;
  my_tlb.size++;
	return;
}

float get_hit_ratio()
{
	return (float)my_tlb.hits / (float)my_tlb.inquiries;
}

//Write to the file in OUT_TLB
void print_tlb()
{
	FILE *tlb_out = fopen(OUT_TLB,"a");
  if(tlb_out == NULL) {
    printf("Error opening file\n");
    exit(1);
  }
	int i;
  for(i=0; i < my_tlb.size; i++) {
    fprintf(tlb_out, "%#07x %#07x\n", my_tlb.data[i][0], my_tlb.data[i][1]);
	}
  for(; i < 8; i++) {
    fprintf(tlb_out, "%d %d\n", -1, -1);
  }
  fprintf(tlb_out, "\n");
  fclose(tlb_out);
	return;
}
