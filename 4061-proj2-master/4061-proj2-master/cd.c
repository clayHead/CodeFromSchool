#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

int main(int argc , char *argv[])
{
  if (argc > 2)
  {
    printf("Incorrect number of args: given %d, expected 1.\n", argc - 1);
    exit(1);
  }
  system("pwd");
  if(chdir(argv[1]) == -1)
  {
    perror("Failed to changed dir \n");
  }
  system("pwd");
  return 0;
}
