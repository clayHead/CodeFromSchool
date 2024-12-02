/* The server that takes user input
 * and sends to either client or
 * broadcasts it across both clients
 */

#define NUM_ARGS 0

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define MSGSZ 256

struct mem_t {
  int status;
  key_t key;
};

struct message {
  long mtype;
  char mtext[MSGSZ];
};

int main(int argc, char** argv) {
  if(argc > NUM_ARGS + 1) {
    printf("Wrong number of args, expected %d, given %d\n",NUM_ARGS, argc-1);
    exit(1);
  }

  key_t key1 = ftok("./client1.c", 'x');
  if(key1 == -1) {
    perror("ERROR unique token could not be created for Client 1.\n");
    exit(1);
  }
  int shmid1 = shmget(key1, sizeof(struct mem_t), IPC_CREAT | 0600);
  if(shmid1 == -1) {
    perror("ERROR shared memory segment could not be created for Client 1.\n");
    exit(1);
  }
  struct mem_t *shmPtr1 = (struct mem_t *) shmat(shmid1, NULL, 0);
  if((int)shmPtr1 == -1) {
    perror("ERROR key could not be attached to Client 1 shared memory.\n");
    exit(1);
  }
  shmPtr1->status = 0;
  shmPtr1->key = key1;
  shmPtr1->status = 1;

  key_t key2 = ftok("./client2.c", 'x');
  if(key2 == -1) {
    perror("ERROR unique token could not be created for Client 1.\n");
    exit(1);
  }
  int shmid2 = shmget(key2, sizeof(struct mem_t), IPC_CREAT | 0600);
  if(shmid2 == -1) {
    perror("ERROR shared memory segment could not be created for Client 2.\n");
    exit(1);
  }
  struct mem_t *shmPtr2 = (struct mem_t *) shmat(shmid2, NULL, 0);
  if((int)shmPtr2 == -1) {
    perror("ERROR key could not be attached to Client 2 shared memory.\n");
    exit(1);
  }
  shmPtr2->status = 0;
  shmPtr2->key = key2;
  shmPtr2->status = 1;

  while(shmPtr1->status != 2 || shmPtr2->status != 2)
    ;

  int msqid1 = msgget(key1, IPC_CREAT | 0666);
  if(msqid1 == -1) {
    perror("ERROR could not create message queue for Client 1.\n");
    exit(1);
  }
  int msqid2 = msgget(key2, IPC_CREAT | 0666);
  if(msqid2 == -1) {
    perror("ERROR could not create message queue for Client 2.\n");
    exit(1);
  }

  char in[3];
  struct message m_out1;
  m_out1.mtype = 1;
  struct message m_in1;
  struct message m_out2;
  m_out2.mtype = 3;
  struct message m_in2;
  while(1) {
    printf("Enter 1 to choose client 1\n");
    printf("      2 to choose client 2\n");
    printf("      3 to broadcast a message\n");
    printf("      0 to exit\n");
    fgets(in, 20, stdin);
    int last = strlen(in) - 1;
    if(in[last] == '\n')
      in[last] = '\0';
    if(in[0] == '1') {
      printf("Enter a message for Client 1: ");
      bzero(m_out1.mtext, MSGSZ);
      fgets(m_out1.mtext, MSGSZ, stdin);
      m_out1.mtype = 1;
      if(msgsnd(msqid1, &m_out1, MSGSZ, 0) == -1) {
        perror("ERROR sending message to Client 1.\n");
        exit(1);
      }
      if(msgrcv(msqid1, &m_in1, sizeof(m_in1), 2, 0) == -1) {
        perror("ERROR receiving message from Client 1.\n");
        exit(1);
      }
      printf("Client 1: %s", m_in1.mtext);
      bzero(&m_in1, sizeof(struct message));
    } else if(in[0] == '2') {
      // Send a signal to client2
      printf("Enter a message for Client 2: ");
      bzero(m_out2.mtext, MSGSZ);
      fgets(m_out2.mtext, MSGSZ, stdin);
      m_out2.mtype = 3;
      if(msgsnd(msqid2, &m_out2, MSGSZ, 0) == -1) {
        perror("ERROR sending message to Client 2.\n");
        exit(1);
      }
      if(msgrcv(msqid2, &m_in2, sizeof(m_in2), 4, 0) == -1) {
        perror("ERROR receiving message from Client 2.\n");
        exit(1);
      }
      printf("Client 2: %s", m_in2.mtext);
      bzero(&m_in2, sizeof(struct message));
    } else if(in[0] == '3') {
      // Broadcast a signal
      printf("Enter a message to broadcast: ");
      bzero(m_out1.mtext, MSGSZ);
      fgets(m_out1.mtext, MSGSZ, stdin);
      m_out1.mtype = 5;
      if(msgsnd(msqid1, &m_out1, MSGSZ, 0) == -1) {
        perror("ERROR broadcasting message to Client 1.\n");
        exit(1);
      }
      if(msgsnd(msqid2, &m_out1, MSGSZ, 0) == -1) {
        perror("ERROR broadcasting message to Client 2.\n");
        exit(1);
      }
    } else if(in[0] == '0') {
      // Close server connection and terminate
      // Also terminate clients.
      bzero(m_out1.mtext, MSGSZ);
      strcpy(m_out1.mtext, "TERMINATE");
      m_out1.mtype = 9;
      if(msgsnd(msqid1, &m_out1, MSGSZ, 0) == -1) {
        perror("ERROR terminating Client 1.\n");
        exit(1);
      }
      if(msgsnd(msqid2, &m_out1, MSGSZ, 0) == -1) {
        perror("ERROR terminating Client 2.\n");
        exit(1);
      }
      return 0;
    } else {
      // Invalid input
      printf("%s is not an option\n", in);
    }
  }
}
