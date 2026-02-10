#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/getproc.h"

int
main(void)
{
  struct procinfo procs[64];
  int n = getprocs((uint64)procs);
  
  if(n < 0) {
    printf("getprocs failed\n");
    exit(1);
  }
  
  const char* state_names[] = {"unused", "used", "sleep", "runble", "run", "zombie"};

  printf("PID\tSTATE\tSIZE\tNAME\n");
  for(int i = 0; i < n; i++) {
    printf("%d\t%s\t%d\t%s\n",
           procs[i].pid,
           state_names[procs[i].state],
           (int)procs[i].sz,
           procs[i].name);
  }
  
  exit(0);
}