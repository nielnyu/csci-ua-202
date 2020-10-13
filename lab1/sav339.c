#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
  char command[50];
  char del[] = "\n";

  while (1) {
    printf("lab1> ");

    command[0] = '\0';
    fgets(command, 50, stdin); // get 50 chars from stdin
    printf("Parent process %d\n", getpid());

    char * ptr = strtok(command, del);
    if (ptr != NULL) strtok(command, del); // remove '\n' from fgets() output

    if (strcmp(command, "printid") == 0 || strcmp(command, "greet") == 0 || strcmp(command, "exit") == 0) {
      if (strcmp(command, "printid") == 0) {
        printf("The ID of the current process is %d\n", getpid());
      } else if (strcmp(command, "greet") == 0) {
        printf("Hello\n");
      } else if (strcmp(command, "exit") == 0) {
        exit(0);
      } else {
        printf("I sense a disturbance in the force...\n");
      }
    } else {
      int pid = fork();
      if (pid == 0) { // child process

        char catcmd[50] = "/bin/";

        char* progname[2]; // second argument of execve
        progname[0] = command;
        progname[1] = NULL;

        printf("Child process %d will execute the command %s...\n", getpid(), command);

        execve(strcat(catcmd, command), progname, NULL);

        printf("Command not found...\n");

        /* if (strcmp(command, "ls") == 0) {
          progname[0] = "ls"; // set progname
          execve("/bin/ls", progname, NULL);
        } else if (strcmp(command, "pwd") == 0) {
          progname[0] = "pwd"; // set progname
          execve("/bin/pwd", progname, NULL);
        } else if (strcmp(command, "ps") == 0) {
          progname[0] = "ps"; // set progname
          execve("/bin/ps", progname, NULL);
        } else if (strcmp(command, "date") == 0) {
          progname[0] = "date"; // set progname
          execve("/bin/date", progname, NULL);
        } else if (strcmp(command, "lscpu") == 0) {
          progname[0] = "lscpu"; // set progname
          execve("/bin/lscpu", progname, NULL);
        } else {
          printf("Command not found...\n");
        } */

        exit(2);
      } // no need for else since we exit regardless
    }

    int status;
    wait(&status); // wait for child to finish
  }
}
