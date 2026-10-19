// Q2: physical memory statistics test
#include "types.h"
#include "stat.h"
#include "user.h"
#include "meminfo.h"

static void
show(char *tag)
{
  struct meminfo mi;
  if(getmeminfo(&mi) < 0){
    printf(1, "getmeminfo failed\n");
    exit();
  }
  printf(1, "[%s] total=%d free=%d user_pages(sz/PGSIZE)=%d resident=%d pt_pages=%d (user-half tables=%d)\n",
         tag, mi.total_pages, mi.free_pages, mi.user_pages,
         mi.resident_pages, mi.pt_pages, mi.pt_user_pages);
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
  int before, after, during;
  char *p;
  int i;

  printf(1, "== Q2: memory statistics ==\n");
  show("start");

  p = sbrk(64 * 4096);
  show("after sbrk(64 pages), untouched");
  for(i = 0; i < 64; i++)
    p[i * 4096] = 1;            // touch every page
  show("after touching all 64 pages");

  before = freepages();
  int pid = fork();
  if(pid < 0){
    printf(1, "fork failed\n");
    exit();
  }
  if(pid == 0){
    show("child");
    exit();
  }
  during = freepages();           // child exists (zombie) but is not reaped yet
  printf(1, "[parent] free before fork=%d, while child is a zombie=%d (used by child=%d)\n",
         before, during, before - during);
  wait();
  after = freepages();
  printf(1, "[parent] free after wait()=%d\n", after);
  if(after == before)
    printf(1, "NO LEAK: free pages returned to %d\n", before);
  else
    printf(1, "LEAK? before=%d after=%d\n", before, after);
  exit();
}
