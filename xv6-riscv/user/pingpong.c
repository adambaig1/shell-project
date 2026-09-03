#include "kernel/types.h"
#include "user/user.h"

int main() {
  int p[2];
  char our_byte = 'P';

  pipe(p);
  int pid = fork();

  if (pid > 0) { // Parent
    write(p[1], &our_byte, 1);
    pid = wait((int *) 0);
    int my_pid = getpid();
    char received_byte;
    read(p[0], &received_byte, 1);
    printf("%d:Received pong, %d\n", my_pid, received_byte);
  }

  else if (pid == 0) { // Child
    char the_byte;
    read(p[0], &the_byte, 1);
    int my_pid = getpid();
    printf("%d:Received ping, %d \n", my_pid, the_byte);
    if (the_byte == 'P')
      the_byte = 'R';
    write(p[1], &the_byte, 1);
    exit(0);
  }
  exit(0);
}
