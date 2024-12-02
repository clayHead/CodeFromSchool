#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

struct process {
  char* executable;
  char** args;
  int arg_count;
};

void append_arg(struct process* proc, char* to_add){
  if(proc->arg_count > 0) {
    char *previous_args[proc->arg_count];
    for(int i = 0; i < proc->arg_count; i++) {
      previous_args[i] = proc->args[i];
    }
    free(proc->args);
    proc->args = (char **)malloc(sizeof(char*) * proc->arg_count + 1);
    for(int i = 0; i < proc->arg_count; i++) {
      proc->args[i] = previous_args[i];
    }
    proc->args[proc->arg_count] = to_add;
  } else {
    proc->args = (char**)malloc(sizeof(char*));
    proc->args[0] = to_add;
  }
  proc->arg_count++;
}

void *execute(void* process_v) {
  struct process* to_exec = process_v;
  // I'm assuming we'll only need
  // execl for paths
  // execlp for files

  // These next lines are only here for testing:
  char* exec_with_null[to_exec->arg_count+1];
  for(int i = 0; i < to_exec->arg_count; i++){
    exec_with_null[i] = to_exec->args[i];
  }
  exec_with_null[to_exec->arg_count] = NULL;
  if(strstr(to_exec->executable, ".o") == NULL){
    execv(to_exec->executable, exec_with_null);
  } else {
    execvp(to_exec->executable, exec_with_null);
  }
  printf("An error occured in execution\n");
  return (void*)0;

  // Note to_exec twice, this is to account for the first input to most
  // command line executions being the executables name. If a file calls
  // on argv[0], it should get its name back.
}


int main(int argc, char *argv[]) {
  int par, cur_index;
  if(argc == 1) {
    printf("Insufficient arguments. Usage: \n");
    printf("Sequential: \n");
    printf("\t./myBashProj (exec) (args) \"&\" (exec) (args)\n");
    printf("Parallel: \n");
    printf("\t./myBashProj -p (exec) (args) \"&\" (exec) (args)\n");
    return 1;
  }

  if(strcmp(argv[1], "-p") == 0) {
    // We run them in parallel
    cur_index = 2;
    par = 1;
  } else {
    // We run them sequentially
    cur_index = 1;
    par = 0;
  }

    // getting ampersand positions
    static int split_positions[15];
    split_positions[0] = cur_index - 1;
    int split_count = 1;
    for(int i = cur_index; i < argc; i++) {
      if(strcmp(argv[i], "&") == 0) {
        split_positions[split_count] = i;
        split_count++;
      }
    }
    struct process processes[split_count];
    for(int i = 0; i < split_count; i++) {
      processes[i].arg_count = 0;
    }
    int exec_count = 0;

    for(int i = cur_index-1; i < argc; i++) {
      if(i == split_positions[exec_count]) {
        // The loop found an "&", so there is an executable on the next one
        processes[exec_count].executable = argv[i+1];
        append_arg(&processes[exec_count],argv[i+1]);
        exec_count++;
        i++;
      } else {
        append_arg(&processes[exec_count-1],argv[i]);
      }
    }

    pid_t pid;
    //pthread_t thread;
    // This for loop will make 2^n-1 copies
    for(int i = split_count-1; i >= 0; i--) {
      pid = fork();

      if(pid) {
        if(par == 0) {
          wait(NULL);
          execute((void*)&processes[i]);
        } else {
          execute((void*)&processes[i]);
          //pthread_create(&thread, NULL, execute, (void*)&processes[i]);
        }
      } else if (pid == 0) {
        // printf("Son\n");
      } else {
        printf("Fork error\n");
      }
    }
    //pthread_join(thread, NULL);
    for(int i = 0; i < split_count; i++) {
      free(processes[i].args);
    }

  return 0;
}

