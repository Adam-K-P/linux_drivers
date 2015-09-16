/* Adam Pinarbasi
 * mem_test_client
 *
 * This is the client program for using the mem_test driver.
 *
 * Instructions on using the driver can be found by using the 'help' command.
 *
 * This program is free software: you can redistribute it and/or modify it 
 * under the terms of the GNU General Public License as published by the free
 * Software Foundation, either version 3 of the License, or (at your option) 
 * any later version.  This program is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General 
 * Public License for more details.  You should have received a copy of the GNU 
 * General Public License along with this program.  If not, see 
 * <http://www.gnu.org/licenses/>. */

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

static void print_help (void) 
{
   printf("\n\n------------------------------------------------------------\n"
          "'q' quits the program\n\n"

          "'p' allows the user to specify a pattern\n"
          "ex: p1111111111111111\n"
          "would specify a pattern of 1111111111111111\n\n"

          "'n' allows the user to specify the number" 
                              "of passes to be performed\n"
          "ex: n4\n"
          "would specify 4 passes on a memory chunk\n\n"

          "each block of memory given a unique two character identifier\n"
          "all identifiers are just a number followed by a letter\n"
          "ex: 1a, 2a, 1b, etc.\n\n"


          "'m' allows the user to specify the start of a memory block"
                  "and its length\n"
          "ex: m018987987876 200\n"
          "specifies the address 018987987876 with length of 200\n"
          "would specify a memory block as being from"
                                  "018987987876 - 018987988076\n\n"

          "the program will then echo the identifier of the memory block\n"
          "'c' allows the user to test for corruption\n"
          "ex: c1a\n"
          "will test memory block 1a for corruption\n"
          "------------------------------------------------------------\n\n");
}

static void primary_loop (const int device) 
{
   char buffer[MAX_SIZE];
   printf("# ");
   while (fgets(buffer, MAX_SIZE, stdin) != NULL) {
      if (*buffer == 'q') break; 

      if (*buffer == 'h') {
         print_help(); 
         printf("# ");
         continue;
      }
      printf("# "); //Just the prompt

      char *input = strdup(buffer);
      if (input[strlen(input) - 1] == '\n')
         input[strlen(input) - 1] = '\0';
      //printf("%s\n", input);

      //include null byte?
      if (write(device, (const void *)input, strlen(input) + 1) < 0)
         error("test_memory", "error writing to file");
      free(input);
   }
}

static void print_greeting (void) 
{
   printf("\n\n------------------------------------------------------------\n"
          "Welcome to mem_test: a program for testing memory corruption\n"
          "To get a list of commands type in 'help'\n"
          "Written by Adam Pinarbasi\n"
          "------------------------------------------------------------\n\n");
}

int main (void) 
{
   const int device = open("/dev/mem_test", O_RDWR);
   if (device < 0) error("main", "Unable to open file");
   print_greeting();
   primary_loop(device);
   close(device);
   return EXIT_SUCCESS;
}
