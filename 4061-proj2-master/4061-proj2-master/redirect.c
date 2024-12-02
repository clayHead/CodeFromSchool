/*
#include<stdio.h>
#include<dirent.h>
#include<sys/stat.h>
#include<string.h>

// Two arguments. First is file and second is a binary value for ">>" or ">" (1 or 0, respectivly)

int main(int argc, char *argv[])
{
	if (argc != 3) {printf("Incorect number of arguments, expected 2, given %d", argc-1); return(1);}

	// Store stdout so that output is redirected back after
	int std_out = dup(fileno(stdout));

	if (argv[2] = 1) {int file = open(argv[1], O_RDWR|O_APPEND|O_CREAT, 0600);}
	else {int file = open(argv[1], O_RDWR|O_CREAT, 0600);}


}
*/

// Code from https://stackoverflow.com/questions/8516823/redirecting-output-to-a-file-in-c
// Outline for what we will need in the shell. Rework for shell

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, const char *argv[])
{
    int out = open("cout.log", O_RDWR|O_CREAT|O_APPEND, 0600);
    if (-1 == out) { perror("opening cout.log"); return 255; }

    int err = open("cerr.log", O_RDWR|O_CREAT|O_APPEND, 0600);
    if (-1 == err) { perror("opening cerr.log"); return 255; }

    int save_out = dup(fileno(stdout));
    int save_err = dup(fileno(stderr));

    if (-1 == dup2(out, fileno(stdout))) { perror("cannot redirect stdout"); return 255; }
    if (-1 == dup2(err, fileno(stderr))) { perror("cannot redirect stderr"); return 255; }

    puts("doing an ls or something now");

    fflush(stdout); close(out);
    fflush(stderr); close(err);

    dup2(save_out, fileno(stdout));
    dup2(save_err, fileno(stderr));

    close(save_out);
    close(save_err);

    puts("back to normal output");

    return 0;
}
