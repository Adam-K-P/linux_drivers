#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_SIZE 80

static void error (const char *function, const char *message) 
{
   printf("error in function: %s - %s\n", function, message);
   exit(EXIT_FAILURE);
}

static void test_memory (int device) 
{
   char buffer[MAX_SIZE];
   while (fgets(buffer, MAX_SIZE, stdin) != NULL) {
      if (*buffer == 'q') break; //no command can start with 'q'
      char *input = strdup(buffer);
      if (write(device, input, strlen(input)) < 0)
         error("test_memory", "error writing to file");
      free(input);
   }
}

int main (void) 
{
   int device = open("/dev/mem_test", O_RDWR);
   if (device < 0) error("main", "Unable to open file");
   test_memory(device);
   close(device);
   return EXIT_SUCCESS;
}

      




