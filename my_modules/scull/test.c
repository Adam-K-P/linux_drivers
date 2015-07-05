/*
   ****************************************
   This file is for testing purposes only
   ****************************************
*/
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main (int argc, char **argv) {
   ssize_t res, rev;
   int fd, string_len = 1;

   for (int i = 1; i < argc; ++i) string_len += strlen(argv[i]);
   char *string = malloc(string_len);
   for (int i = 1; i < argc; ++i) strcat(string, argv[i]);

   fd = open("/dev/scull", O_RDWR);
   char *buffer = malloc(string_len);
   res = write(fd, (const void *)string, string_len);
   if (res < 0) return EXIT_FAILURE;

   char *buf = malloc(string_len);
   rev = read(fd, buf, string_len);
   if (rev < 0) return EXIT_FAILURE;
   printf("Read string: %s from kernel\n", buf);

   close(fd);
   return EXIT_SUCCESS;
}
