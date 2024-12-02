#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int main(int argc , char *argv[])
{
  if (argc < 2)
  {
    printf("Incorrect number of args: given %d, expected 1.\n", argc - 1);
    exit(1);
  }

  else if (argc == 3)
  {
    FILE *pf;
    char line[100];
    pf = fopen(argv[2],"r");
    while(fscanf(pf , "%[^\n]\n" , line)!=EOF)
    {
      if(strstr(line , argv[1]) !=NULL)
      {
        printf("%s\n" , line);
      }
      else
      {
        continue;
      }
     }
    fclose(pf);
  }
  else
  {
    char line[100];
    while(fscanf(stdin , "%[^\n]\n" , line)!=EOF)
    {
      if(strstr(line , argv[1]) !=NULL)
      {
        printf("%s\n" , line);
      }
      else
      {
        continue;
      }
    }
    fclose(stdin);
  }
  return 0;
}
