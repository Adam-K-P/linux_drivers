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

int main (void) {
   ssize_t res, rev;
   int fd;
   fd = open("/dev/scull", O_RDWR);
   char *string = "hello, world";
   char *buffer = malloc(strlen(string) + 1);
   res = write(fd, (const void *)string, 1);
   if (res < 0) return 1;
   rev = read(fd, buffer, 1);
   printf("Read string: %s from kernel\n", buffer);
   close(fd);
}
