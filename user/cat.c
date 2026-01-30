#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include <stdbool.h>

char buf[512];

void
cat(int fd, int print_line_num)
{
  int n;

  int line_num = 1;
  int at_line_start = 1;
  if (!print_line_num) {
    while((n = read(fd, buf, sizeof(buf))) > 0) {
      if (write(1, buf, n) != n) {
        fprintf(2, "cat: write error\n");
        exit(1);
      }
    }
    if(n < 0){
      printf("cat: read error\n");
      exit(1);
    }
    return;
  }

  while((n = read(fd, buf, sizeof(buf))) > 0) {

    int i = 0;
    while (i < n) {
      if (at_line_start) {
        printf("%d\t", line_num);
        line_num++;
        at_line_start = 0;
      }
      
      int j = i;
      while (j < n && buf[j] != '\n') {
        ++j;
      }
      int chunk_len = (j < n) ? (j - i + 1) : (n - i);
      if (write(1, buf + i, chunk_len) != chunk_len) {
        printf("cat: write error\n");
        exit(1);
      }
      if (j < n) at_line_start = 1;
      i += chunk_len;
    }

  }
  if(n < 0){
    printf("cat: read error\n");
    exit(1);
  }
}

bool
compareArg(const char *inp, const char *arg) 
{
  while (*inp && *arg) {
    if (*inp != *arg) return 0;
    inp++;
    arg++;
  }
  return *inp == *arg;
}

int
main(int argc, char *argv[])
{
  int fd, i;

  if(argc <= 1){
    cat(0, 0);
    exit(0);
  }

  int print_line_num = 0;
  for (int i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') {
      if (compareArg(argv[i], "-n")) print_line_num = 1;
      else {
        printf("No option for %s\n", argv[i]);
      }
    }
  }

  for(i = 1; i < argc; i++){
    if (argv[i][0] == '-') continue;
    if((fd = open(argv[i], O_RDONLY)) < 0){
      printf("cat: cannot open %s\n", argv[i]);
      exit(1);
    }
    cat(fd, print_line_num);
    close(fd);
  }
  exit(0);
}
