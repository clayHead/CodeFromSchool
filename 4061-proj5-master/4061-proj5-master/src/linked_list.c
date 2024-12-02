#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <sys/time.h>
#include "linked_list.h"

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t sequence_lock = PTHREAD_MUTEX_INITIALIZER;
int line_buffer = 0;
int sequence_number = 0;
char* fileName;
FILE* file;
#define NUM_ARGS 2
void helpMe() {
    int temp; //temp contains line number for the this thread
    pthread_mutex_lock(&lock);
    temp = line_buffer;
    line_buffer++;
    pthread_mutex_unlock(&lock);
    // get specific line buffer
    char* line = malloc(sizeof(char)*LINE_SIZE);
    int counter = 0;
    while (fgets(line, sizeof(line), file)) {
        if(temp==counter) {
            break;
        }
        counter++;
    }
    if (line == NULL ) {
        return;
    } else {
      node *temp1 = (node*) malloc(sizeof(node *));
      temp1 = create_node(temp,line);
      pthread_mutex_lock(&sequence_lock);
      temp1->seq_no = sequence_number;
      temp1->content = line;
      //insert node into the linked list
      printf("Inserting node\n");
      insert(&head,temp1);
      sequence_number++;
      pthread_mutex_unlock(&sequence_lock);
    }
}

//Read a particular line from file
//param 1: file name, param 2: line_no

//Traverse the linked list with the head as the argument
void traversal(node * head) {
  FILE *fptr =fopen(fileName,"w");
  while(head!=NULL){
    fprintf(fptr,"%d, %d, %s\n",head->seq_no,head->line_no,head->content);
    head = head->next;
  }
}

//Insert a node created using create_node into the linked list
//param 1: head address, param 2: node
//TODO; Chase
void insert(node **head, node *in){
  node* cur_node = *head;
  if(cur_node == NULL){
    cur_node = in;
    return;
  } else if(cur_node->line_no > in->line_no) {
    in->next = cur_node;
    return;
  }
  while(cur_node->next != NULL) {
    // While we can still go to the next node
    if(cur_node->next->line_no < in->line_no) {
      // if the next node's line number is less than
      // the inserted one, we go further down
      cur_node = cur_node->next;
    } else {
      // The next node's line number is greater
      // than the inserted one. Stop going down
      break;
    }
  }
  // insert the node into the linked list
  if(cur_node->next == NULL) {
    // in is the last element of the list
    cur_node->next = in;
  } else if(cur_node->next->next != NULL) {
    // in is inserted in the middle of the list
    in->next = cur_node->next->next;
    cur_node->next = in;
  }
}

//Create a new node to be inserted into the linked list
//param 1: line_no; param 2: line content
node* create_node(int number, char * cont)
{
    node * newnode = (struct linked_node*)malloc(sizeof(struct linked_node));
    newnode->seq_no= 0;;
    newnode->line_no = number;
    newnode->next=NULL;
    return newnode;


}

int main(int argc, char** argv) {
  if (argc < NUM_ARGS + 1) {
    printf("Wrong number of args, expected %d, given %d\n", NUM_ARGS, argc - 1);
		exit(1);
	}
  // get the input from command line argument
  fileName = argv[1];
  int numThreads = atoi(argv[2]);
  file = fopen(fileName,"r");
  if(file == NULL) {
    printf("ERROR: could not open file.\n");
    exit(1);
  }
  // chase you can check if num threads is between 1 to 16 if you want
  if (numThreads < 1 || numThreads > 16) {
    printf("ERROR: Illegal number of threads. \n");
    exit(1);
  }
  // Create threads.
  pthread_t pool[numThreads];
  // Let us create threads
  for (int i=0; i < numThreads; ++i) {
    pthread_create(&pool[i], NULL,(void*)helpMe, NULL);
	}
  // let us join threads
  for (int i = 0; i < numThreads; ++i) {
    pthread_join(pool[i],NULL);
  }
  traversal(head);
  //TODO: free list
  fclose(file);
  return 0;
}
