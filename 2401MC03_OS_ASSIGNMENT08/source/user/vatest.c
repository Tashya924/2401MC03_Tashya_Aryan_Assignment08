// Q1: virtual -> physical translation test
#include "types.h"
#include "stat.h"
#include "user.h"

int g = 42;            // global variable (data segment)

int
main(void)
{
  int local = 7;                 // local variable (stack)
  char *heap = malloc(100);      // heap block
  int pa;

  printf(1, "== Q1: va2pa test ==\n");
  printf(1, "global : va=0x%x\n", (int)&g);
  pa = va2pa((int)&g);
  printf(1, "         pa=0x%x\n", pa);
  printf(1, "local  : va=0x%x\n", (int)&local);
  pa = va2pa((int)&local);
  printf(1, "         pa=0x%x\n", pa);
  printf(1, "malloc : va=0x%x\n", (int)heap);
  pa = va2pa((int)heap);
  printf(1, "         pa=0x%x\n", pa);
  printf(1, "invalid: va=0x7FFFFFFF\n");
  pa = va2pa(0x7FFFFFFF);
  printf(1, "         pa=%d (expected -1)\n", pa);
  printf(1, "kernel : va=0x80000000 (no PTE_U)\n");
  pa = va2pa(0x80000000);
  printf(1, "         pa=%d (expected -1)\n", pa);

  printf(1, "\n== fork: same VA, different PA ==\n");
  int ppa = va2pa((int)&g);
  int pid = fork();
  if(pid < 0){
    printf(1, "fork failed\n");
    exit();
  }
  if(pid == 0){
    int cpa = va2pa((int)&g);
    printf(1, "child : &g va=0x%x pa=0x%x\n", (int)&g, cpa);
    exit();
  }
  wait();
  printf(1, "parent: &g va=0x%x pa=0x%x\n", (int)&g, ppa);
  exit();
}
