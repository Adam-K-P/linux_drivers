#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void error(const char *function, const char *message) 
{
   printf("error in function: %s - %s\n", function, message);
   exit(EXIT_FAILURE);
}


int main (void) 
{
   int err = open("/dev/mem_test", O_RDWR);
   if (err < 0) error("main", "Unable to open file");
   close(err);
   return EXIT_SUCCESS;
}

      




