#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include <stdbool.h>

char buf[512];

bool
compareArg(const char *a, const char *b)
{
  while (*a && *b) {
    if (*a != *b)
      return false;
    a++;
    b++;
  }
  return *a == *b;
}

void
printResult(int l, int w, int c, const char *name,
            bool cnt_l, bool cnt_w, bool cnt_c)
{
  if (cnt_l) printf("%d ", l);
  if (cnt_w) printf("%d ", w);
  if (cnt_c) printf("%d ", c);
  if (name[0])
    printf("%s", name);
  printf("\n");
}

void
wc(int fd, const char *name, bool cnt_l, bool cnt_w, bool cnt_c)
{
  int n;
  int l = 0, w = 0, c = 0;
  int inword = 0;

  while ((n = read(fd, buf, sizeof(buf))) > 0) {
    for (int i = 0; i < n; i++) {
      c++;

      if (buf[i] == '\n')
        l++;

      if (buf[i] == ' ' || buf[i] == '\n' || buf[i] == '\t' ||
          buf[i] == '\r' || buf[i] == '\v') {
        inword = 0;
      } else if (!inword) {
        w++;
        inword = 1;
      }
    }
  }

  if (n < 0) {
    fprintf(2, "wc: read error\n");
    exit(1);
  }

  printResult(l, w, c, name, cnt_l, cnt_w, cnt_c);
}

int
main(int argc, char *argv[])
{
  int fd;
  bool cnt_l = false, cnt_w = false, cnt_c = false;
  bool saw_file = false;

  // Parse options
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] != '-')
      continue;

    if (compareArg(argv[i], "-l")) cnt_l = true;
    else if (compareArg(argv[i], "-w")) cnt_w = true;
    else if (compareArg(argv[i], "-c")) cnt_c = true;
    else {
      fprintf(2, "wc: unknown option %s\n", argv[i]);
      exit(1);
    }
  }

  // Default: -l -w -c
  if (!cnt_l && !cnt_w && !cnt_c)
    cnt_l = cnt_w = cnt_c = true;

  // Process files
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      if (argv[i][1] == '\0') {
        wc(0, "", cnt_l, cnt_w, cnt_c);
        saw_file = true;
      }
      continue;
    }

    if ((fd = open(argv[i], O_RDONLY)) < 0) {
      fprintf(2, "wc: cannot open %s\n", argv[i]);
      exit(1);
    }

    wc(fd, argv[i], cnt_l, cnt_w, cnt_c);
    close(fd);
    saw_file = true;
  }

  if (!saw_file)
    wc(0, "", cnt_l, cnt_w, cnt_c);

  exit(0);
}
