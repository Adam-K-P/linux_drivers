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
   fprintf(stderr, "error in function: %s - %s\n", function, message);
   exit(EXIT_FAILURE);
}

/* where the user interacts with the program
   'q' quits the program

   'p' allows the user to specify a pattern
   ex: p1111111111111111
   would specify a pattern of 1111111111111111

   'n' allows the user to specify the number of passes to be performed
   ex: n4 
   would specify 4 passes on a memory chunk

   each block of memory given a unique two character identifier 
   all identifiers are just a number followed by a letter
   ex: 1a, 2a, 1b, etc.

   'm' allows the user to specify the start of a memory block
   ex: m018987987876
   specifies the address 018987987876
   must then be followed by the length of the memory block
   ex: 200
   would specify a memory block as being from 018987987876 - 018987988076

   the program will then echo the identifier of the memory block
   'c' allows the user to test for corruption
   ex: c1a
   will test memory block 1a for corruption
*/
static void primary_loop (const int device) 
{
   char buffer[MAX_SIZE];
   while (fgets(buffer, MAX_SIZE, stdin) != NULL) {
      if (*buffer == 'q') break; 
      char *input = strdup(buffer);

      bool newc = false;
      if (input[strlen(input) - 1] == '\n') {
         input[strlen(input) - 1] = '\0';
         newc = true;
      }

      if (write(device, (const void *)input, 
                        strlen(input) - (newc ? 1 : 0)) < 0)
         error("test_memory", "error writing to file");
      free(input);
   }
}

int main (void) 
{
   const int device = open("/dev/mem_test", O_RDWR);
   if (device < 0) error("main", "Unable to open file");
   primary_loop(device);
   close(device);
   return EXIT_SUCCESS;
}

      




