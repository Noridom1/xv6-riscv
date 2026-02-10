#ifndef GETPROC_H
#define GETPROC_H

// Process states
#ifndef PROCSTATE_ENUM
#define PROCSTATE_ENUM
enum procstate { UNUSED, USED, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };
#endif

// Structure to hold process information
struct procinfo {
  int pid;                // Process ID
  enum procstate state;   // Process state
  uint64 sz;              // Size of process memory (bytes)
  char name[16];          // Process name
};

#endif
