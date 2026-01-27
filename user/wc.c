#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include <stdbool.h>

char buf[512];

void
printResult(int l, int w, int c, const char *name,
            bool cnt_l, bool cnt_w, bool cnt_c)
{
  int printed = 0;
  if (cnt_l) { printf("%d", l); printed = 1; }
  if (cnt_w) { printf("%s%d", printed ? " " : "", w); printed = 1; }
  if (cnt_c) { printf("%s%d", printed ? " " : "", c); }
  printf("%s%s\n", name[0] ? " " : "", name);
}

void
wc(int fd, char *name, bool cnt_l, bool cnt_w, bool cnt_c)
{
  int i, n;
  int l, w, c, inword;

  l = w = c = 0;
  inword = 0;
  while((n = read(fd, buf, sizeof(buf))) > 0){
    for(i=0; i<n; i++){
      c++;
      if(buf[i] == '\n')
        l++;
      if(strchr(" \r\t\n\v", buf[i]))
        inword = 0;
      else if(!inword){
        w++;
        inword = 1;
      }
    }
  }
  if(n < 0){
    printf("wc: read error\n");
    exit(1);
  }
  printResult(l, w, c, name, cnt_l, cnt_w, cnt_c);
}

bool
compareArg(const char *inp, const char *arg) 
{
  while (*inp && *arg) {
    if (*inp != *arg) return false;
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
    // wc(0, "");
    exit(0);
  } 
  bool l = 0, w = 0, c = 0;

  for(i = 1; i < argc; i++){
    if (argv[i][0] == '-') {
      if (compareArg(argv[i], "-l")) l = true;
      if (compareArg(argv[i], "-w")) w = true;
      if (compareArg(argv[i], "-c")) c = true;
    }
  }

  if (!l && !w && !c) {
    l = 1;
    w = 1;
    c = 1;
  }

  for (int i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') continue;

    if((fd = open(argv[i], O_RDONLY)) < 0){
      printf("wc: cannot open %s\n", argv[i]);
      exit(1);
    }
    wc(fd, argv[i], l, w, c);
    close(fd);
    
  }
  exit(0);
}
