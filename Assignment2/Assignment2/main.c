#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define STD_IN 0
#define STD_OUT 1
#define READ 0
#define WRITE 1
#define BUFFER 512
void fork_pipe(char*);

int main() {
  char* char_list = calloc(sizeof(char), BUFFER);
  read(STD_IN, char_list, BUFFER);
  if (strlen(char_list) == 1) {
    printf("%c", char_list[0]);
    exit(0);
  }
  else {
    fork_pipe(char_list);
  }
}

void fork_pipe(char* orig_str) {
  int pipe_child1[2];
  int pipe_child2[2];
  pid_t child1;
  pid_t child2;
  if (pipe (pipe_child1)) {
   perror("Piping child 1 failed");
   exit(1);
  }
  if (pipe (pipe_child2)) {
    perror("Piping child 2 failed");
    exit(1);
  }
  if ((child1 = fork()) == 0) {
    //child 1 process
    // dup pipes
    if (dup2(pipe_child1[READ], STD_IN) == -1) {
      perror("Dup child 1 READ failed");
      exit(1);
    }
    if (dup2(pipe_child1[WRITE], STD_OUT) == -1) {
      perror("Dup child 1 WRITE failed");
      exit(1);
    }
    execl("assignment2", "assignment2", (char*) NULL);
    exit(0);
  }
  else if (child1 == -1) {
    perror("Forking child 1 failed");
    exit(1);
  }
  else if ((child2 = fork()) == 0) {
    //child 2 proccess
    //dup pipes
    if (dup2(pipe_child2[READ], STD_IN) == -1) {
      perror("Dup child 2 READ failed");
      exit(1);
    }
    if (dup2(pipe_child2[WRITE], STD_OUT) == -1) {
      perror("Dup child 2 WRITE failed");
      exit(1);
    }
    execl("assignment2", "assignment2", (char*) NULL);
    exit(0);
  }
  else if (child2 == -1) {
    perror("Forking child 2 failed");
    exit(1);
  }
  else {
    //parent process
    // write_to_pipe
    write(pipe_child1[WRITE], orig_str, sizeof(char) * strlen(orig_str)/2);
    close(pipe_child1[WRITE]);
    write(pipe_child2[WRITE], &orig_str[strlen(orig_str)/2], sizeof(char) *  (strlen(orig_str)-(strlen(orig_str)/2)));
    close(pipe_child2[WRITE]);
    wait(NULL);
    wait(NULL);
    //read from pipe
    char* char_list1 = calloc(sizeof(char), BUFFER);
    char* char_list2 = calloc(sizeof(char), BUFFER);
    read(pipe_child1[READ], char_list1, BUFFER);
    read(pipe_child2[READ], char_list2, BUFFER);
    int i = 0;
    int j = 0;
    while (i < strlen(char_list1) && j < strlen(char_list2)) {
      if ((int)char_list1[i] < (int)char_list2[j]) {
        printf("%c", char_list1[i]);
        i++;
      }
      else {
        printf("%c", char_list2[j]);
        j++;
      }
    }
    while (i < strlen(char_list1)) {
      printf("%c", char_list1[i]);
      i++;
    }
    while (j < strlen(char_list2)) {
      printf("%c", char_list2[j]);
      j++;
    }
    exit(0);
  }
}
