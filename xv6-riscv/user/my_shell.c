#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"


int getcmd(char *buf, int nbuf);
void run_command(char *buf, int nbuf, int *pcp)__attribute__((noreturn));


/* Read a line of characters from stdin. */
int getcmd(char *buf, int nbuf) {
  printf(">>> ");  // command prompt
  memset(buf, 0, nbuf); // clearing the buffer
  if (gets(buf, nbuf) == 0) { // getting user input
    fprintf(2, "error: failed to read input\n");
    return 1;
  } 
  if (buf[0] == 0) { // if user exits 
    return -1;
  }
  return 0;
}

/*
  A recursive function which parses the command
  at *buf and executes it.
*/
__attribute__((noreturn))
void run_command(char *buf, int nbuf, int *pcp) {

  /* Useful data structures and flags. */
  char *arguments[10];
  int numargs = 0;
  /* Word start/end */
  int ws = 1;
  int we = 0;

  int redirection_left = 0;
  int redirection_right = 0;
  char *file_name_l = 0;
  char *file_name_r = 0;

  int p[2];
  int pipe_cmd = 0;

  int sequence_cmd = 0;

  int i = 0;
  int pi = -1; // this is a pipe index counter

  /* Parse the command character by character. */
  for (; i < nbuf; i++) {

    /* Parse the current character and set-up various flags:
       sequence_cmd, redirection, pipe_cmd and similar. */

    if (buf[i] == ' ' || buf[i] == '\n' || buf[i] == 0){ // parsing whitespace
      if (we){ // this if statement clause allows for new arguments to be added, and to disregard whitespace
        buf[i] = 0;
        arguments[numargs++] = &buf[ws];
        ws = i + 1;
        we = 0;
      }
    } else if (buf[i] == '<') { // parsing the <
      redirection_left = 1;
      buf[i++] = 0;

      while(buf[i] == ' ') i++;// skipping the whitespace
      file_name_l = &buf[i];// capture the start of the filename and point to it

      // below, is the code to add the terminating char '\0' at the end of the filename so it can be parsed properly
      while (buf[i] != ' ' && buf[i] != '\n' && buf[i] != 0) i++;
      buf[i] = 0;

    } else if (buf[i] == '>') { // parsing the >
      redirection_right = 1;
      buf[i++] = 0;

      while(buf[i] == ' ') i++;// skipping the whitespace
      file_name_r = &buf[i];// capture the start of the filename and point to it

      // below, is the code to add the terminating char '\0' at the end of the filename so it can be parsed properly
      while (buf[i] != ' ' && buf[i] != '\n' && buf[i] != 0) i++;
      buf[i] = 0;

    } else if (buf[i] == '|') { // parsing |'s
      pipe_cmd = 1;
      buf[i++] = 0;
      pi = i + 1;// setting the pipe index for future processing
      break;
    } else if (buf[i] == ';'){ // parsing sequencez
      sequence_cmd = 1;
      buf[i++] = 0;
    } else { // parsing new words
      if (!we) ws = i;
      we = 1;
    }

  }

  /*
    Sequence command. Continue this command in a new process.
    Wait for it to complete and execute the command following ';'.
  */
  if (sequence_cmd) {
    if (fork() == 0) {
      run_command(buf + i, nbuf - i, pcp); // Recursively execute command after ';'
    }
    wait(0);
  }

  /*
    If this is a redirection command,
    tie the specified files to std in/out.
  */
  if (redirection_left) {
    close(0); // close stdin
    if (open(file_name_l, O_RDONLY) < 0) {// opening the file entered, if failed, an error message is outputted
      fprintf(2, "error: can't open file %s\n", file_name_l);
      exit(1);
    }
  }
  if (redirection_right) {
    close(1); // close stdout
    if (open(file_name_r, O_WRONLY | O_CREATE) < 0) {// creating/writing file entered, if failed, an error message it outputted
      fprintf(2, "error: can't write to file %s\n", file_name_r);
      exit(1);
    }
  }

  /* Parsing done. Execute the command. */

  /*
    If this command is a CD command, write the arguments to the pcp pipe
    and exit with '2' to tell the parent process about this.
  */
  if (strcmp(arguments[0], "cd") == 0) {
    if (numargs < 2) {
      fprintf(2, "error: cd needs argument\n");
      exit(1);
    }else {
      write(pcp[1], arguments[1], strlen(arguments[1]) + 1); // writing through pcp pipe the dir name
      exit(2); // custom exit code to show cd command
    }
  } else {
    /*
      Pipe command: fork twice. Execute the left hand side directly.
      Call run_command recursion for the right side of the pipe.
    */

    if (pipe_cmd) {
      if (pipe(p) < 0){
        fprintf(2, "error: pipe failed\n");
        exit(1);
      }

      if (fork() == 0) {
        close(1); 
        dup(p[1]); // this basically makes it so that the outputs of the child process will go into the pipe instead of the console
        close(p[0]); 
        close(p[1]);
        arguments[numargs] = 0;
        exec(arguments[0], arguments);// executing the left hand side of the pipe
        exit(1);
      }

      if (fork() == 0){
        close(0); 
        dup(p[0]);
        close(p[0]); 
        close(p[1]); 
        run_command(buf + pi, nbuf - pi, pcp); // this recursively calls the run_command function, to handle the command after the |
        exit(1);
      }

      close(p[0]);
      close(p[1]);
      wait(0);
      wait(0);
      exit(0);
    }
    else {
      arguments[numargs] = 0;
      exec(arguments[0], arguments);
      exit(1);
    }
  }

  exit(0);
}

int main(void) {
  char buf[100];  
  int pcp[2];
  pipe(pcp);
  

  /* Read and run input commands. */
  while(getcmd(buf, sizeof(buf)) >= 0){
    if(fork() == 0)
      run_command(buf, 100, pcp);

    /*
      Check if run_command found this is
      a CD command and run it if required.
    */
    int child_status;
    wait(&child_status); // this is so that main() waits until child process is complete and to store status in variable

    // handling cd command
    if (child_status == 2) {
      char cdp[100];
      read(pcp[0], cdp, sizeof(cdp));// reading from the child process, getting the dir name
      if (chdir(cdp) < 0){
        fprintf(2, "error: can't cd %s\n", cdp);
      }
    }
  }
  exit(0);
}