#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include <stdbool.h>

char buf[512];

void
cat(int fd, int print_line_num, int *line_num)
{
  int n;
  int at_line_start = 1;

  while ((n = read(fd, buf, sizeof(buf))) > 0) {
    for (int i = 0; i < n; i++) {
      if (print_line_num && at_line_start) {
        printf("%d\t", (*line_num)++);
        at_line_start = 0;
      }

      if (write(1, &buf[i], 1) != 1) {
        fprintf(2, "cat: write error\n");
        exit(1);
      }

      if (buf[i] == '\n')
        at_line_start = 1;
    }
  }

  if (n < 0) {
    fprintf(2, "cat: read error\n");
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
  int fd;
  int print_line_num = 0;
  int saw_file = 0;
  int line_num = 1; 

  // Parse options
  for (int i = 1; i < argc; i++) {
    if (compareArg(argv[i], "-n"))
      print_line_num = 1;
    else if (argv[i][0] == '-') {
      fprintf(2, "cat: unknown option %s\n", argv[i]);
      exit(1);
    }
  }

  // Process files
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-')
      continue;

    if ((fd = open(argv[i], O_RDONLY)) < 0) {
      fprintf(2, "cat: cannot open %s\n", argv[i]);
      exit(1);
    }

    cat(fd, print_line_num, &line_num);
    close(fd);
    saw_file = 1;
  }

  // stdin
  if (!saw_file)
    cat(0, print_line_num, &line_num);

  exit(0);
}
