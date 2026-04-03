#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/getproc.h"

int
main(void)
{
  struct procinfo procs[64];
  // Ask kernel to fill this array with active process snapshots.
  // Return value is number of entries written.
  int n = getprocs((uint64)procs);
  
  if(n < 0) {
    printf("getprocs failed\n");
    exit(1);
  }
  
  // Must match enum procstate order in kernel/getproc.h.
  const char* state_names[] = {"unused", "used", "sleep", "runble", "run", "zombie"};

  printf("PID\tSTATE\tSIZE\tNAME\n");
  for(int i = 0; i < n; i++) {
    // Each row corresponds to one struct procinfo copied from kernel.
    printf("%d\t%s\t%d\t%s\n",
           procs[i].pid,
           state_names[procs[i].state],
           (int)procs[i].sz,
           procs[i].name);
  }
  
  exit(0);
}