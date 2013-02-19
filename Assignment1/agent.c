#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/types.h>

void fork_smokers();
void smoke(int);

int main(int argc, int *argv[]) {
  printf("AGENT INFO: \n");
  char hostname[HOST_NAME_MAX];
  gethostname(hostname, HOST_NAME_MAX);
  printf("HOSTNAME: %s\n", hostname);
  printf("PID: %d\n", getpid());
  printf("UID: %d\n", getuid());
  time_t now = time(NULL);
  printf("TIME: %s\n", ctime(&now));
  char paper[12];
  char tobacco[12];
  char matches[12];
  int P = 3;
  int T = 2;
  int M = 2;

  sprintf(paper,"%d", P);
  sprintf(tobacco, "%d", T);
  sprintf(matches, "%d", M);

  setenv("PAPER", paper,1);
  setenv("TOBACCO", tobacco,1);
  setenv("MATCHES", matches,1);

  printf("PAPER: %d\n", atoi(getenv("PAPER")));
  printf("TOBACCO: %d\n", atoi(getenv("TOBACCO")));
  printf("MATCHES: %d\n", atoi(getenv("MATCHES")));
  fork_smokers();
}

void fork_smokers() {
  printf("FORKING THREE SMOKERS!\n");
  int f = fflush(NULL);
  if (f != 0) {
    perror("Flush");
  }
  pid_t fork1 = fork();
  if (fork1 == 0) {
    smoke(1);
  }
  else if (fork1 == -1) {
    perror("Cannot fork.");
    exit(EXIT_FAILURE);
  }
  else {
    int s = usleep(200000);
    if (s > 0) {
      perror("Sleep");
    }
    pid_t fork2 = fork();
    if (fork2 == 0) {
      smoke(2);
    }
    else if (fork2 == -1) {
      perror("Cannot fork.");
      exit(EXIT_FAILURE);
    }
    else {
      int s = usleep(200000);
      if (s > 0) {
        perror("Sleep");
      }
      pid_t fork3 = fork();
      if (fork3 == 0) {
        smoke(3);
      }
      else if (fork3 == -1) {
        perror("Cannot fork.");
        exit(EXIT_FAILURE);
      }
      else {
        int i = 0;
        for (;i < 3; i++) {
          if(wait(NULL) == -1) {
            perror("Wait");
          }
        }
        printf("AGENT IS DONE!\n");
      }
    }
  }
  exit(0);
}

void smoke(int resource)
{
  while(1)
  {
    printf("SMOKER S%d: %d\n", resource, getpid());
    int paper = atoi(getenv("PAPER"));
    if (paper < resource) {
      printf("NO PAPER!\n");
      exit(0);
    }
    else {
      printf("PAPER IS AVAILABLE! P: %d\n", paper-resource+1);
      paper -= 3;
      char p[12];
      sprintf(p, "%d", paper);
      setenv("PAPER", p, 1);
    }
    int tobacco = atoi(getenv("TOBACCO"));
    if (tobacco < resource) {
      printf("NO TOBACCO!\n");
      exit(0);
    }
    else {
      printf("TOBACCO IS AVAILABLE! T: %d\n", tobacco-resource+1);
      tobacco -= 3;
      char t[12];
      sprintf(t, "%d", tobacco);
      setenv("TOBACCO", t, 1);
    }
    int matches = atoi(getenv("MATCHES"));
    if (matches < resource) {
      printf("NO MATCHES!\n");
      exit(0);
    }
    else {
      printf("MATCHES ARE AVAILABLE! M: %d\n", matches-resource+1);
      matches -= 3;
      char m[12];
      sprintf(m, "%d", matches);
      setenv("MATCHES", m, 1);
    }
    printf("SMOKING!\n");
    int f = fflush(NULL);
    if (f != 0) {
      perror("Flush error");
    }
    int s = sleep(1);
    if (s > 0) {
      perror("Sleep error");
    }
  }
}
