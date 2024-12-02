/* Client 1 to send and receive
 * messages from the server
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

struct message {
  long mtype;
  char mtext[256];
};

struct mem_t {
  int status;
  key_t key;
};

extern int errno;

int main(int argc, char** argv) {
  if(argc < NUM_ARGS + 1) {
    printf("Wrong number of args, expected %d, given %d\n", NUM_ARGS, argc-1);
    exit(1);
  }

  int errnum;
  // int sockfd, n;
  // struct sockaddr_in serv_addr;

  key_t key = ftok("./client1.c", 'x');
  if(key == -1) {
    perror("ERROR cannot create key.\n");
    exit(1);
  }
  int shmid = shmget(key, sizeof(struct mem_t), 0600);
  if(shmid == -1) {
    perror("ERROR cannot access shared memory.\n");
    exit(1);
  }
  struct mem_t *shm = (struct mem_t*) shmat(shmid, NULL, 0);
  if((int)shm == -1) {
    perror("ERROR cannot attach to shared memory.\n");
    exit(1);
  }
  while(shm->status == 0)
    ;
  shm->status = 2;

  int msqid = msgget(key, 0666 | IPC_CREAT);
  if(msqid == -1) {
    perror("ERROR cannot access message queue.\n");
    exit(1);
  }
  struct message m_in;
  struct message m_out;
  while(1) {
    if(msgrcv(msqid, &m_in, sizeof(m_in), 1, IPC_NOWAIT) > 0) {
      printf("Server: %s", m_in.mtext);
      printf("Enter your response: ");
      bzero(m_in.mtext, 256);
      bzero(m_out.mtext, 256);
      fgets(m_out.mtext, 256, stdin);
      m_out.mtype = 2;
      if(msgsnd(msqid, &m_out, sizeof(m_out), 0) < 0) {
        errnum = errno;
        perror("ERROR message could not send to server.\n");
        fprintf(stderr, "Value of errno: %s\n", strerror(errnum));
        exit(1);
      }
    } else if(msgrcv(msqid, &m_in, sizeof(m_in), 5, IPC_NOWAIT) > 0) {
      printf("Server: %s", m_in.mtext);
      bzero(m_in.mtext, 256);
    } else if(msgrcv(msqid, &m_in, sizeof(m_in), 9, IPC_NOWAIT) > 0) {
      // Logging off
      // Clean queue first
      if(msgctl(msqid, IPC_RMID, NULL) == -1) {
        perror("Message queue could not be deleted.\n");
        exit(1);
      }
      return 0;
    }
  }
}
