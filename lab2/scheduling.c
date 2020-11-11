#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct process {
  int processID;
  int cpuTime;
  int ioTime;
  int arrivalTime;

  int blocking;
  int running;
  int readyTime;
  int endTime;
  int timeBlocking;
  int timeOnCPU;
  int halfTime;
} PROCESS;

void printProcess(PROCESS p) {
  printf("PROCESS {\n    processID: %d\n    cpuTime: %d\n    ioTime: %d\n    arrivalTime: %d\n\n    blocking: %d\n    running: %d\n    readyTime: %d\n    endTime: %d\n    timeBlocking: %d\n    timeOnCPU: %d\n    halfTime: %d\n}\n\n", p.processID, p.cpuTime, p.ioTime, p.arrivalTime, p.blocking, p.running, p.readyTime, p.endTime, p.timeBlocking, p.timeOnCPU, p.halfTime);
}

int getRemainingCPUTime(PROCESS p) {
  if (p.timeBlocking > 0) {
    return p.halfTime - p.timeOnCPU;
  } else {
    return p.halfTime + p.halfTime - p.timeOnCPU;
  }
}

PROCESS * extractProcesses(FILE * infile, int size) {
  PROCESS * items = malloc(sizeof(PROCESS) * size);

  for (int i = 0; i < size; i++) {
    // scan into items
    fscanf(infile, "%d %d %d %d", &items[i].processID, &items[i].cpuTime, &items[i].ioTime, &items[i].arrivalTime);
    items[i].blocking = 0;
    items[i].running = 0;
    items[i].readyTime = -1;
    items[i].endTime = -1;
    items[i].timeBlocking = 0;
    items[i].timeOnCPU = 0;
    items[i].halfTime = (items[i].cpuTime + 1) / 2; // thank you integer division
  }

  return items;
}

// First-Come-First-Served
void FCFS(FILE * outfile, PROCESS * proclist, int size) {
  int readyqueue[size];

  for (int i = 0; i < size; i++) {
    readyqueue[i] = -1;
  }

  int readyqueuehead = 0;
  int readyqueuetail = 0;
  int waiting = size;
  int blocking = 0;
  int running = -1;
  int terminated = 0;

  int cpuTime = 0;

  int timer = -1;

  char * turnaroundstring = malloc(2048 * sizeof(char));

  char * outstring = malloc(512 * sizeof(char));

  while (terminated < size) {
    timer++;
    strcpy(outstring, "");

    // printf("\nTime %d, Waiting %d, Blocking %d, Running index %d, Terminated %d\n\n", timer, waiting, blocking, running, terminated);
    // printf("QUEUE: head -> %d || tail -> %d \n\n", readyqueuehead, readyqueuetail);

    if (waiting != 0) {
      // enqueue
      for (int i = 0; i < size; i++) {
        // printf("%d %d %d %d\n", proclist[i].processID, proclist[i].cpuTime, proclist[i].ioTime, proclist[i].arrivalTime);
        if (proclist[i].arrivalTime == timer) {
          proclist[i].readyTime = timer;
          waiting--;
          readyqueue[readyqueuetail] = i;
          readyqueuetail = (readyqueuetail + 1) % size;
        }
      }
    }

    if (blocking != 0) {
      // enqueue
      for (int i = 0; i < size; i++) {
        if (proclist[i].blocking == 1) {
          if (proclist[i].timeBlocking == proclist[i].ioTime) {
            proclist[i].readyTime = timer;
            proclist[i].blocking = 0;
            proclist[i].timeOnCPU = 0;
            blocking--;
            readyqueue[readyqueuetail] = i;
            readyqueuetail = (readyqueuetail + 1) % size;
          } else {
            proclist[i].timeBlocking++;
          }
        }
      }
    }

    // print ready queue
    // for (int i = 0; i < size; i++) {
    //   printf("%d || ", readyqueue[i]);
    // }

    int needtodequeue = 0;

    if (running == -1) {
      needtodequeue = 1;
    } else {
      // check if running process blocks or terminates
      PROCESS * curr = &proclist[running];

      if (curr->timeOnCPU == curr->halfTime) {
        // do blocking
        if (curr->timeBlocking > 0) {
          // is terminated
          terminated++;
          running = -1;
          curr->running = 0;
          curr->endTime = timer;
        } else {
          blocking++;
          running = -1;
          curr->running = 0;
          curr->blocking = 1;
          curr->timeBlocking = 1;
        }
        needtodequeue = 1;
      } else {
        // keep running
        curr->timeOnCPU++;
        cpuTime++;
      }
    }

    if (needtodequeue > 0 && readyqueuehead != readyqueuetail) {
      int readyTime = proclist[readyqueue[readyqueuehead]].readyTime;

      // check for same ready time but lower processID
      int mindex = readyqueuehead;
      int min = proclist[readyqueue[readyqueuehead]].processID;

      // printf("\nDEQUEUE: ");

      int i = 1;
      while(readyqueue[(readyqueuehead + i) % size] >= 0 && readyTime == proclist[readyqueue[(readyqueuehead + i) % size]].readyTime) {
        // printf("%d || ", readyqueue[(readyqueuehead + i) % size]);
        // printf("\n\n%d %d\n\n", min, mindex);
        if (proclist[readyqueue[(readyqueuehead + i) % size]].processID < min) {
          mindex = (readyqueuehead + i) % size;
          min = proclist[readyqueue[mindex]].processID;
        }

        i++;
      }

      // printf("\n\n");

      // swap to lowest
      if (mindex != readyqueuehead) {
        int tmp = readyqueue[readyqueuehead];
        readyqueue[readyqueuehead] = readyqueue[mindex];
        readyqueue[mindex] = tmp;
      }

      running = readyqueue[readyqueuehead];
      proclist[running].timeOnCPU++;
      cpuTime++;
      proclist[running].running = 1;

      readyqueue[readyqueuehead] = -1;

      readyqueuehead = (readyqueuehead + 1) % size;
    }

    for (int i = 0; i < size; i++) {
      // printProcess(proclist[i]);
      if (proclist[i].arrivalTime > timer || proclist[i].endTime > 0) {
        continue;
      } else {
        char proc[25];
        sprintf(proc, "%d:", i);

        if (proclist[i].running == 1) strcat(proc, "running ");
        else if (proclist[i].blocking == 1) strcat(proc, "blocked ");
        else if (proclist[i].readyTime >= 0) strcat(proc, "ready ");

        strcat(outstring, proc);
      }
    }

    if (terminated != size) fprintf(outfile, "%d: %s\n", timer, outstring);
  }

  for (int i = 0; i < size; i++) {
    char procstring[25];
    sprintf(procstring, "Turnaround process %d: %d\n", i, proclist[i].endTime - proclist[i].arrivalTime);

    strcat(turnaroundstring, procstring);
  }

  fprintf(outfile, "\nFinishing time: %d\nCPU Utilization: %.2f\n%s", timer - 1, cpuTime / (timer * 1.0), turnaroundstring);

  free(outstring);

  free(turnaroundstring);
}

// Round-Robin-Quantum-2
void RR2(FILE * outfile, PROCESS * proclist, int size) {
  int readyqueue[size];

  for (int i = 0; i < size; i++) {
    readyqueue[i] = -1;
  }

  int readyqueuehead = 0;
  int readyqueuetail = 0;
  int waiting = size;
  int blocking = 0;
  int running = -1;
  int timeRunning = 0;
  int terminated = 0;

  int cpuTime = 0;

  int timer = -1;

  char * turnaroundstring = malloc(2048 * sizeof(char));

  char * outstring = malloc(512 * sizeof(char));

  while (terminated < size) {
    timer++;
    strcpy(outstring, "");

    // printf("\nTime %d, Waiting %d, Blocking %d, Running index %d, Terminated %d\n\n", timer, waiting, blocking, running, terminated);
    // printf("QUEUE: head -> %d || tail -> %d \n\n", readyqueuehead, readyqueuetail);

    if (waiting != 0) {
      // enqueue
      for (int i = 0; i < size; i++) {
        // printf("%d %d %d %d\n", proclist[i].processID, proclist[i].cpuTime, proclist[i].ioTime, proclist[i].arrivalTime);
        if (proclist[i].arrivalTime == timer) {
          proclist[i].readyTime = timer;
          waiting--;
          readyqueue[readyqueuetail] = i;
          readyqueuetail = (readyqueuetail + 1) % size;
        }
      }
    }

    if (blocking != 0) {
      // enqueue
      for (int i = 0; i < size; i++) {
        if (proclist[i].blocking == 1) {
          if (proclist[i].timeBlocking == proclist[i].ioTime) {
            proclist[i].readyTime = timer;
            proclist[i].blocking = 0;
            proclist[i].timeOnCPU = 0;
            blocking--;
            readyqueue[readyqueuetail] = i;
            readyqueuetail = (readyqueuetail + 1) % size;
          } else {
            proclist[i].timeBlocking++;
          }
        }
      }
    }

    // print ready queue
    // for (int i = 0; i < size; i++) {
    //   printf("%d || ", readyqueue[i]);
    // }

    int needtodequeue = 0;

    if (running == -1) {
      needtodequeue = 1;
    } else {
      // check if running process blocks or terminates
      PROCESS * curr = &proclist[running];

      if (curr->timeOnCPU == curr->halfTime) {
        // do blocking
        if (curr->timeBlocking > 0) {
          // is terminated
          terminated++;
          running = -1;
          timeRunning = 0;
          curr->running = 0;
          curr->endTime = timer;
        } else {
          blocking++;
          running = -1;
          timeRunning = 0;
          curr->running = 0;
          curr->blocking = 1;
          curr->timeBlocking = 1;
        }
        needtodequeue = 1;
      } else if (timeRunning == 2) {
        // enqueue running
        readyqueue[readyqueuetail] = running;
        readyqueuetail = (readyqueuetail + 1) % size;
        running = -1;
        timeRunning = 0;
        curr->running = 0;
        curr->readyTime = timer;
        needtodequeue = 1;
      } else {
        // keep running
        curr->timeOnCPU++;
        cpuTime++;
        timeRunning++;
      }
    }

    if (needtodequeue > 0 && readyqueue[readyqueuehead] != -1) {
      int readyTime = proclist[readyqueue[readyqueuehead]].readyTime;

      // check for same ready time but lower processID
      int mindex = readyqueuehead;
      int min = proclist[readyqueue[readyqueuehead]].processID;

      // printf("\nDEQUEUE: ");

      int i = 1;
      while(readyqueue[(readyqueuehead + i) % size] >= 0 && readyTime == proclist[readyqueue[(readyqueuehead + i) % size]].readyTime) {
        // printf("%d || ", readyqueue[(readyqueuehead + i) % size]);
        // printf("\n\n%d %d\n\n", min, mindex);
        if (proclist[readyqueue[(readyqueuehead + i) % size]].processID < min) {
          mindex = (readyqueuehead + i) % size;
          min = proclist[readyqueue[mindex]].processID;
        }

        i++;
      }

      // printf("\n\n");

      // swap to lowest
      if (mindex != readyqueuehead) {
        int tmp = readyqueue[readyqueuehead];
        readyqueue[readyqueuehead] = readyqueue[mindex];
        readyqueue[mindex] = tmp;
      }

      running = readyqueue[readyqueuehead];
      proclist[running].timeOnCPU++;
      cpuTime++;
      timeRunning = 1;
      proclist[running].running = 1;

      readyqueue[readyqueuehead] = -1;

      readyqueuehead = (readyqueuehead + 1) % size;
    }

    for (int i = 0; i < size; i++) {
      // printProcess(proclist[i]);
      if (proclist[i].arrivalTime > timer || proclist[i].endTime > 0) {
        continue;
      } else {
        char proc[25];
        sprintf(proc, "%d:", i);

        if (proclist[i].running == 1) strcat(proc, "running ");
        else if (proclist[i].blocking == 1) strcat(proc, "blocked ");
        else if (proclist[i].readyTime >= 0) strcat(proc, "ready ");

        strcat(outstring, proc);
      }
    }

    if (terminated != size) fprintf(outfile, "%d: %s\n", timer, outstring);
  }

  for (int i = 0; i < size; i++) {
    char procstring[25];
    sprintf(procstring, "Turnaround process %d: %d\n", i, proclist[i].endTime - proclist[i].arrivalTime);

    strcat(turnaroundstring, procstring);
  }

  fprintf(outfile, "\nFinishing time: %d\nCPU Utilization: %.2f\n%s", timer - 1, cpuTime / (timer * 1.0), turnaroundstring);

  free(outstring);

  free(turnaroundstring);
}

// Shortest-Remaining-Job-First
void SJF(FILE * outfile, PROCESS * proclist, int size) {
  int readylist[size];

  for (int i = 0; i < size; i++) {
    readylist[i] = 0;
  }

  int waiting = size;
  int blocking = 0;
  int running = -1;
  int terminated = 0;

  int cpuTime = 0;

  int timer = -1;

  char * turnaroundstring = malloc(2048 * sizeof(char));

  char * outstring = malloc(512 * sizeof(char));

  while (terminated < size) {
    timer++;
    strcpy(outstring, "");

    // printf("\nTime %d, Waiting %d, Blocking %d, Running index %d, Terminated %d\n\n", timer, waiting, blocking, running, terminated);

    if (waiting != 0) {
      // enqueue
      for (int i = 0; i < size; i++) {
        // printf("%d %d %d %d\n", proclist[i].processID, proclist[i].cpuTime, proclist[i].ioTime, proclist[i].arrivalTime);
        if (proclist[i].arrivalTime == timer) {
          proclist[i].readyTime = timer;
          waiting--;
          readylist[i] = 1;
        }
      }
    }

    if (blocking != 0) {
      // enqueue
      for (int i = 0; i < size; i++) {
        if (proclist[i].blocking == 1) {
          if (proclist[i].timeBlocking == proclist[i].ioTime) {
            proclist[i].readyTime = timer;
            proclist[i].blocking = 0;
            proclist[i].timeOnCPU = 0;
            blocking--;
            readylist[i] = 1;
          } else {
            proclist[i].timeBlocking++;
          }
        }
      }
    }

    // print ready queue
    // for (int i = 0; i < size; i++) {
    //   printf("%d || ", readyqueue[i]);
    // }

    if (running >= 0) {
      // check if running process blocks or terminates
      PROCESS * curr = &proclist[running];

      if (curr->timeOnCPU == curr->halfTime) {
        // do blocking
        if (curr->timeBlocking > 0) {
          // is terminated
          terminated++;
          readylist[running] = 0;
          running = -1;
          curr->running = 0;
          curr->endTime = timer;
        } else {
          blocking++;
          readylist[running] = 0;
          running = -1;
          curr->running = 0;
          curr->blocking = 1;
          curr->timeBlocking = 1;
        }
      } else {
        readylist[running] = 1;
        running = -1;
        curr->running = 0;
      }
    }

    int mindex = -1;
    int minremain = 2147483647; // INT_MAX, without the extra header :)
    int minid = 2147483647;

    for (int i = 0; i < size; i++) {
      if (readylist[i] < 1) continue;
      else if (proclist[i].arrivalTime > timer) continue;
      else {
        int remain = getRemainingCPUTime(proclist[i]);
        if (remain <= minremain) {
          if (remain == minremain) {
            if (proclist[i].processID < minid) {
              mindex = i;
              minremain = remain;
              minid = proclist[i].processID;
            }
          } else {
            mindex = i;
            minremain = remain;
            minid = proclist[i].processID;
          }
        }
      }
    }

    // printf("Index of process with shortest remaining CPU time: %d\nRemaining CPU time for this process: %d\n\n", mindex, minremain);

    if (mindex >= 0) {
      running = mindex;
      proclist[running].timeOnCPU++;
      cpuTime++;
      proclist[running].running = 1;

      readylist[mindex] = -1;
    }

    for (int i = 0; i < size; i++) {
      // printProcess(proclist[i]);
      if (proclist[i].arrivalTime > timer || proclist[i].endTime > 0) {
        continue;
      } else {
        char proc[25];
        sprintf(proc, "%d:", i);

        if (proclist[i].running == 1) strcat(proc, "running ");
        else if (proclist[i].blocking == 1) strcat(proc, "blocked ");
        else if (proclist[i].readyTime >= 0) strcat(proc, "ready ");

        strcat(outstring, proc);
      }
    }

    if (terminated != size) fprintf(outfile, "%d: %s\n", timer, outstring);
  }

  for (int i = 0; i < size; i++) {
    char procstring[25];
    sprintf(procstring, "Turnaround process %d: %d\n", i, proclist[i].endTime - proclist[i].arrivalTime);

    strcat(turnaroundstring, procstring);
  }

  fprintf(outfile, "\nFinishing time: %d\nCPU Utilization: %.2f\n%s", timer - 1, cpuTime / (timer * 1.0), turnaroundstring);

  free(outstring);

  free(turnaroundstring);
}

int main( int argc, char *argv[] ) {
  const char * WARN = "\n\n./scheduling [filename] [algo]\n\nfilename: The path to the input file, maximum size 1024 bytes\nalgo:     0, 1, or 2, where\n          0 - First-Come-First-Served (nonpreemptive)\n          1 - Round-Robin with quantum 2\n          2 - Shorted remaining job first (preemptive)\n\nPlease try again.\n";

  if (argc != 3) {
    // error if wrong number of arguments
    printf("This program requires exactly two arguments.%s", WARN);
    return 1;
  }

  int algo;

  if (strcmp(argv[2], "0") == 0) {
    algo = 0;
  } else if (strcmp(argv[2], "1") == 0) {
    algo = 1;
  } else if (strcmp(argv[2], "2") == 0) {
    algo = 2;
  } else {
    algo = -1;
    // error if incorrect algo arg
    printf("The scheduling algorithm needs to be either 0, 1, or 2.%s", WARN);
  }

  FILE * infile;
  char lenstr[10];

  if ((infile = fopen(argv[1], "r"))) {
    fscanf(infile, "%s", lenstr);
  } else {
    // error if file doesn't exist or can't be read
    printf("The input file you provided doesn't exist.%s", WARN);
    return 1;
  }

  const int len = atoi(lenstr);

  if (len == 0) {
    // error if no processes
    printf("No processes listed.%s", WARN);
    return 1;
  }

  PROCESS * proclist;
  proclist = extractProcesses(infile, len);

  // for (int i = 0; i < len; i++) {
  //   printf("%d %d %d %d\n", proclist[i].processID, proclist[i].cpuTime, proclist[i].ioTime, proclist[i].arrivalTime);
  // }

  FILE * outfile;
  const char delim[4] = ".";
  char * fname = strtok(argv[1], delim);

  char outname[50];
  sprintf(outname, "%s-%d.txt", fname, algo);

  outfile = fopen(outname, "w");

  if (algo == 0) {
    FCFS(outfile, proclist, len);
  } else if (algo == 1) {
    RR2(outfile, proclist, len);
  } else if (algo == 2) {
    SJF(outfile, proclist, len);
  } else {
    printf("I sense a disturbance in the force...\n");
    return 1;
  }


  free(proclist);

  return 0;
}
