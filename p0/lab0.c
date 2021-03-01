/*
NAME: Conner Yang
EMAIL: conneryang@g.ucla.edu
ID: 905417287
*/

#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void sigsegv_handler(int sig)
{
  if (sig == SIGSEGV)
  {
    fprintf(stderr, "A segmentation fault occured\n");
    exit(4);
  }
}

int main(int argc, char** argv)
{
  int option;
  int segmentation_flag = 0;
  int in_fd = 0;
  int ou_fd = 1;

  static struct option options[] = {
    {"input",    required_argument, 0, 'i'},
    {"output",   required_argument, 0, 'o'},
    {"segfault", no_argument, 0, 's'},
    {"catch",    no_argument, 0, 'c'},
    {0, 0, 0, 0}
  };
    
  while (1)
  {
    option = getopt_long(argc, argv, "i:o:sc", options, NULL);
    if (option == -1) break;
    switch (option)
    {
      case 'i':
	in_fd = open(optarg, O_RDONLY);
	if (in_fd >= 0) {
	  close(0);
	  dup(in_fd);
	  close(in_fd);
	}
	else
	{
	  fprintf(stderr, "--input option error, unable to open file %s", optarg);
	  exit(2);
	}
	break;
      case 'o':
	ou_fd = creat(optarg, 0666);
        if (ou_fd >= 0) {
          close(1);
          dup(ou_fd);
          close(ou_fd);
        }
        else
        {
          fprintf(stderr, "--output option error, unable to open file %s", optarg);
	  exit(3);
        }
	break;
      case 's':
	segmentation_flag = 1;
	break;
      case 'c':
	signal(SIGSEGV, sigsegv_handler);
	break;
      default:
	fprintf(stderr, "Incorrect option, available options are: --input=filename --ouptut=filename --segfault --catch\n");
	exit(1);
    }

	  // Creates a segmentation fault if the create segmentation was flagged
    if (segmentation_flag == 1)
    {
      char* ptr = NULL;
      *ptr = 'a';
    }
  }

  char buffer;
  while (read(0, &buffer, sizeof(char)) > 0)
  {
    if (write(1, &buffer, sizeof(char)) < 0)
    {
      fprintf(stderr, "Error writing to output: %s\n", strerror(errno));
      exit(3);
    }
  }

  close(0);
  close(1);
  exit(0);
}
