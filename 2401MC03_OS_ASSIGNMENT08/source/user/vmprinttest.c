// Q3: page table dump test
#include "types.h"
#include "stat.h"
#include "user.h"

int
main(void)
{
  char *p;
  int i;

  printf(1, "===== 1. at start-up =====\n");
  vmprint();

  p = sbrk(8 * 4096);
  // With lazy sbrk (Q4) the new pages only get frames when touched.
  for(i = 0; i < 8; i++)
    p[i * 4096] = 1;
  printf(1, "\n===== 2. after sbrk(8*4096) and touching the 8 pages =====\n");
  vmprint();

  sbrk(-4096);
  printf(1, "\n===== 3. after sbrk(-4096) =====\n");
  vmprint();
  exit();
}
