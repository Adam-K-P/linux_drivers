#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void error (const char *function, const char *message) 
{
   printf("error in function: %s - %s\n", function, message);
   exit(EXIT_FAILURE);
}

static void test_memory (int file) 
{
   while (true) {
      char *input = NULL;
      size_t input_size = 0;
      while (true) {
         char inc = getchar();
         if (inc == EOF || inc == '\n') break;
         input = realloc(input, ++input_size);
         input = strncat(input, &inc, 1);
         if (input == NULL) 
            error("test_memory", "can't allocate memory");
      }
      printf("%s\n", input);
      if (input != NULL) write(file, input, input_size);
      break; //FIXME until I can figure this thing out ...
   }
}

int main (void) 
{
   int file = open("/dev/mem_test", O_RDWR);
   if (file < 0) error("main", "Unable to open file");
   test_memory(file);
   close(file);
   return EXIT_SUCCESS;
}

      




