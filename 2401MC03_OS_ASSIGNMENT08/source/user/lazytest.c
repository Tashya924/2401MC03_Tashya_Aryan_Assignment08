// Q4: lazy allocation test
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "meminfo.h"

#define MB (1024 * 1024)

static void
show(char *tag)
{
  struct meminfo mi;
  getmeminfo(&mi);
  printf(1, "[%s] free=%d sz/PGSIZE=%d resident=%d\n",
         tag, mi.free_pages, mi.user_pages, mi.resident_pages);
}

static int
freepages(void)
{
  struct meminfo mi;
  getmeminfo(&mi);
  return mi.free_pages;
}

int
main(void)
{
  char *p;
  int i, f1, f2;

  printf(1, "== Q4: lazy allocation ==\n");
  show("start");

  p = sbrk(32 * MB);
  if(p == (char*)-1){
    printf(1, "sbrk(32MB) failed\n");
    exit();
  }
  show("after sbrk(32MB), nothing touched");

  // touch 5 widely spaced pages
  for(i = 0; i < 5; i++)
    p[i * 6 * MB] = 'a' + i;
  show("after touching 5 pages 6MB apart");
  for(i = 0; i < 5; i++)
    if(p[i * 6 * MB] != 'a' + i)
      printf(1, "BAD DATA at page %d\n", i);
  if(p[3 * MB] != 0)
    printf(1, "page not zero filled!\n");

  // fork with holes in the address space
  f1 = freepages();
  int pid = fork();
  if(pid < 0){
    printf(1, "fork failed\n");
    exit();
  }
  if(pid == 0){
    p[1 * MB] = 'z';       // child faults in a fresh page of its own
    if(p[0] != 'a')
      printf(1, "child lost parent's data\n");
    exit();
  }
  wait();
  f2 = freepages();
  printf(1, "fork+exit+wait: free before=%d after=%d -> %s\n", f1, f2,
         f1 == f2 ? "no leak" : "LEAK");

  // system call with an untouched buffer: kernel touches lazy page
  char *buf = p + 20 * MB;       // never touched
  int fd = open("README", O_RDONLY);
  int n = read(fd, buf, 100);
  close(fd);
  printf(1, "read() into untouched buffer: %d bytes, first chars \"%c%c%c\"\n",
         n, buf[0], buf[1], buf[2]);
  write(1, p + 25 * MB, 0);      // zero-length write from untouched memory
  int pp[2];
  pipe(pp);
  write(pp[1], "hi", 2);
  read(pp[0], p + 27 * MB, 2);   // pipe read into untouched buffer
  printf(1, "pipe read into untouched buffer: \"%c%c\"\n", p[27 * MB], p[27 * MB + 1]);
  close(pp[0]);
  close(pp[1]);
  show("after syscalls on untouched buffers");

  // shrink: frees only pages that exist
  f1 = freepages();
  sbrk(-32 * MB);
  f2 = freepages();
  printf(1, "sbrk(-32MB): free %d -> %d (freed %d real pages)\n", f1, f2, f2 - f1);
  show("after shrink");

  // invalid access: child touching address >= sz must die, parent survives
  pid = fork();
  if(pid == 0){
    char *bad = (char*)(0x70000000);
    printf(1, "child: touching 0x70000000 (>= sz), should be killed\n");
    *bad = 1;
    printf(1, "child: SURVIVED (BUG)\n");
    exit();
  }
  wait();
  // access below stack: the guard page
  pid = fork();
  if(pid == 0){
    char x;
    char *guard = (char*)((((int)&x) & ~0xFFF) - 4096);
    printf(1, "child: touching stack guard page %x, should be killed\n", (int)guard);
    *guard = 1;
    printf(1, "child: SURVIVED (BUG)\n");
    exit();
  }
  wait();
  printf(1, "parent still alive after children were killed\n");
  show("end");
  exit();
}
