#include<stdio.h>
#include<dirent.h>
#include<sys/stat.h>
#include<string.h>

int main(int argc , char *argv[])
{
  // Parse input path
  // Open Dir
  // Recurse or loop through with readdir
  // readdir return struct with name string and id
  // parse filename and run stat
  // Use stat to read filesize and add to total
  // Print total filesize

  if (argc != 2) {
    printf("Incorrect Number of arguments. Expected 1, given %d\n", argc-1);
    return 1;
  }

  int total_size = 0;

  DIR *dr = opendir(argv[1]);

  if (dr == NULL) {
    printf("Could not open directory %s\n", argv[1]);
    return 1;
  }

  struct dirent *de;

  while ((de = readdir(dr)) != NULL) {
    if (de->d_name[0] != 46) {
      char path[80];
      strcpy(path,argv[1]);
      strcat(path,"/");
      strcat(path, de->d_name);

      struct stat file_stat;
      if (stat(path,&file_stat) < 0) {
        return 1;
      }

      total_size = total_size + file_stat.st_size;
    }
  }

  printf("Total size: %d\n", total_size);

  return 0;
}
