#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "util.h"
#include <string.h>

extern char *commands[];

struct command {
  char* tokens[25];
  int tok_count;
};

void trim(struct command* coms, int com_count) {
  for(int i = 0; i < com_count; i++) {
    for(int j = 0; j < coms[i].tok_count; j++) {
      coms[i].tokens[j] = trimwhitespace(coms[i].tokens[j]);
    }
  }
}

int change_dir(char* input) {
  char* split[20];
  int length = parse_line(input, split, " ");
  split[1] = trimwhitespace(split[1]);
  if(length > 2){
    return 1;
  } else if(length == 1) {
    return 0;
  } else {
    return chdir(split[1]);
  }
}

void execute(struct command cmd) {
  enum command_type command_num = get_command_type(cmd.tokens[0]);
  if(command_num == ERROR) {
    // The command is not a local implementation
    execvp(cmd.tokens[0], cmd.tokens);
    printf("An error occurred when executing %s\n", cmd.tokens[0]);
    exit(1);
  } else {
    // Will execute all local implemented commands
    execv(commands[command_num], cmd.tokens);
    printf("An error occurred when executing %s\n", commands[command_num]);
    exit(1);
  }
}

void redirect_trunc(char* input, struct command* coms) {
  char* split[2];
  parse_line(input, split, ">");
  coms[0].tok_count = parse_line(split[0], coms[0].tokens, " ");
  coms[1].tok_count = parse_line(split[1], coms[1].tokens, " ");
  int com_count = 2;
  trim(coms, com_count);

  // coms[1] should only be the output file, no more
  int status;
  int pid = fork();
  if(pid == 0) {
    int out_file = open(coms[1].tokens[0], O_WRONLY | O_TRUNC | O_CREAT);
    dup2(out_file, 1);
    dup2(out_file, 2);
    lseek(out_file, 0, SEEK_SET);
    close(out_file);
    execute(coms[0]);
  }
  wait(&status);
}

void redirect_ap(char* input, struct command* coms) {
  char* split[2];
  parse_line(input, split, ">>");
  coms[0].tok_count = parse_line(split[0], coms[0].tokens, " ");
  coms[1].tok_count = parse_line(split[1], coms[1].tokens, " ");
  int com_count = 2;
  trim(coms, com_count);

  // coms[1] should only be the output file, no more
  int status;
  int pid = fork();
  if(pid == 0) {
    int out_file = open(coms[1].tokens[0], O_WRONLY | O_CREAT);
    dup2(out_file, 1);
    dup2(out_file, 2);
    lseek(out_file, 0, SEEK_END);
    close(out_file);
    execute(coms[0]);
  }
  wait(&status);
}

void pipe_2(char* input, struct command* coms){
  // We have to pipe within the input
  int status;
  char* split[2];
  pid_t pid1, pid2;
  int fd[2];
  pipe(fd);
  parse_line(input, split, "|");
  coms[0].tok_count = parse_line(split[0], coms[0].tokens, " ");
  coms[1].tok_count = parse_line(split[1], coms[1].tokens, " ");
  int com_count = 2;
  trim(coms, com_count);

  pid1 = fork();
  if(pid1 == 0) {
    dup2(fd[1], 1);
    close(fd[0]);
    execute(coms[0]);
  }
  pid2 = fork();
  if(pid2 == 0) {
    dup2(fd[0], 0);
    close(fd[1]);
    execute(coms[1]);
  }
  close(fd[0]);
  close(fd[1]);
  wait(&status);
  wait(&status);
}

void pipe_re(char* input, struct command* coms){
  // We have to pipe within the input
  int status;
  int ap;
  char* split1[2];
  char* split2[2];
  pid_t pid1, pid2;
  int fd[2];
  int out_file;

  parse_line(input, split1, "|");
  if(strstr(split1[1], ">>")) {
    parse_line(split1[1], split2, ">>");
    ap = 1;
  } else {
    parse_line(split1[1], split2, ">");
    ap = 0;
  }
  coms[0].tok_count = parse_line(split1[0], coms[0].tokens, " ");
  coms[1].tok_count = parse_line(split2[0], coms[1].tokens, " ");
  coms[2].tok_count = parse_line(split2[1], coms[2].tokens, " ");

  int com_count = 3;
  trim(coms, com_count);

  if(ap) {
    out_file = open(coms[2].tokens[0], O_WRONLY | O_CREAT);
  } else {
    out_file = open(coms[2].tokens[0], O_WRONLY | O_TRUNC | O_CREAT);
  }
  close(out_file);

  pipe(fd);
  pid1 = fork();
  if(pid1 == 0) {
    dup2(fd[1], 1);
    close(fd[0]);
    execute(coms[0]);
  }
  pid2 = fork();
  if(pid2 == 0) {
    dup2(fd[0], 0);
    close(fd[1]);
    dup2(out_file, 1);
    dup2(out_file, 2);
    if(ap) {
      lseek(out_file, 0, SEEK_END);
    } else {
      lseek(out_file, 0, SEEK_SET);
    }
    close(out_file);
    execute(coms[1]);
  }
  /*
    int out_file = open(coms[1].tokens[0], O_WRONLY | O_CREAT);
    dup2(out_file, 1);
    dup2(out_file, 2);
    lseek(out_file, 0, SEEK_END);
    close(out_file);
    execute(coms[0]);
  */
  close(fd[0]);
  close(fd[1]);
  wait(&status);
  wait(&status);
}

int main () {
  char* current_dir;
  int size = 256;
  char buffer[256];
  pid_t pid;
  int status;

  struct command coms[10]; // allocated memory for up to 10 commands
  int com_count = 0;

  while(1) {
    buffer[0] = '\0';
    char* input = (char*)malloc(sizeof(char) * 256);
    current_dir = getcwd(buffer, size);
    printf("[4061-shell]%s $ ", current_dir);

    fgets(input, size, stdin);

    // Exit case is given first:
    if(strcmp(input, "exit\n") == 0) {
      return 0;
    } else if(strstr(input, "cd")) {
      change_dir(input);
    } else {
      if(strstr(input, "|") && (strstr(input, ">") || strstr(input, ">>"))) {
        // there is a combination of pipe and redirect
        pipe_re(input, coms);
      } else if(strstr(input, "|")) {
        // there is only a pipe
        pipe_2(input, coms);
      } else if(strstr(input, ">>")) {
        // there is only a redirect append
        redirect_ap(input, coms);
      } else if(strstr(input, ">")) {
        // there is only a redirect truncate
        redirect_trunc(input, coms);
      } else {
        // We have a basic, single argument executable
        coms[0].tok_count = parse_line(input, coms[0].tokens, " ");
        com_count = 1;
        trim(coms, com_count);
        pid = fork();
        if(pid == 0) { /* child */
          execute(coms[0]);
        }
        wait(&status);
      }
    }
    free(input);
  }
}
