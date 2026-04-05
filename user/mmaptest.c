#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void
delay(int ticks)
{
  int start = uptime();
  while(uptime() - start < ticks)
    ;
}

void
test_basic()
{
  printf("TEST 1: basic mmap\n");

  char *p = (char*) mmap();
  if(p == 0){
    printf("FAIL: mmap returned null\n");
    exit(1);
  }

  *p = 42;
  if(*p != 42){
    printf("FAIL: write/read mismatch\n");
    exit(1);
  }

  printf("PASS\n");
  munmap((uint64)p);
}

void
test_fork_shared()
{
  printf("TEST 2: fork shared memory\n");

  char *p = (char*) mmap();
  *p = 10;

  int pid = fork();
  if(pid == 0){
    *p = 99;
    exit(0);
  }

  wait(0);

  if(*p == 99){
    printf("PASS\n");
  } else {
    printf("FAIL: expected 99, got %d\n", *p);
    exit(1);
  }

  munmap((uint64)p);
}

void
test_child_munmap()
{
  printf("TEST 3: child munmap\n");

  char *p = (char*) mmap();
  *p = 55;

  int pid = fork();
  if(pid == 0){
    munmap((uint64)p);
    exit(0);
  }

  wait(0);

  if(*p == 55){
    printf("PASS\n");
  } else {
    printf("FAIL: memory corrupted\n");
    exit(1);
  }

  munmap((uint64)p);
}

void
test_parent_munmap_first()
{
  printf("TEST 4: parent munmap first\n");

  char *p = (char*) mmap();
  *p = 77;

  int pid = fork();
  if(pid == 0){
    delay(10); // ensure parent unmaps first
    if(*p == 77){
      printf("PASS (child still sees value)\n");
    } else {
      printf("FAIL: child memory broken\n");
    }
    exit(0);
  }

  munmap((uint64)p);
  wait(0);
}

void
test_exit_cleanup()
{
  printf("TEST 5: exit cleanup\n");

  int pid = fork();
  if(pid == 0){
    char *p = (char*) mmap();
    *p = 33;
    exit(0);
  }

  wait(0);
  printf("PASS (no crash)\n");
}

void
test_stress()
{
  printf("TEST 6: stress fork\n");

  char *p = (char*) mmap();
  *p = 1;

  for(int i = 0; i < 5; i++){
    int pid = fork();
    if(pid == 0){
      *p += 1;
      exit(0);
    }
  }

  for(int i = 0; i < 5; i++){
    wait(0);
  }

  printf("Final value: %d (should be >= 1)\n", *p);
  munmap((uint64)p);
}

int
main(int argc, char *argv[])
{
  printf("=== MMAP TEST START ===\n");

  test_basic();
  test_fork_shared();
  test_child_munmap();
  test_parent_munmap_first();
  test_exit_cleanup();
  test_stress();

  printf("=== ALL TESTS DONE ===\n");
  exit(0);
}