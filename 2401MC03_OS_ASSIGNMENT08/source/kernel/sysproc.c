#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "meminfo.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  struct proc *p = myproc();
  uint newsz;

  if(argint(0, &n) < 0)
    return -1;
  addr = p->sz;
  if(n >= 0){
    // Q4: lazy growth -- only move the break; no kalloc, no mapping.
    // Pages are allocated by the T_PGFLT handler when first touched.
    newsz = addr + n;
    if(newsz < addr || newsz >= KERNBASE)
      return -1;
    p->sz = newsz;
  } else {
    // Shrinking: never go below the heap start (would eat the stack).
    // growproc -> deallocuvm frees only pages that are actually mapped
    // (holes left by lazy allocation are skipped) and flushes the TLB.
    if((uint)(-n) > addr || addr + n < p->heapstart)
      return -1;
    if(growproc(n) < 0)
      return -1;
  }
  return addr;
}

// Q1: int va2pa(int va).  Walk the caller's page table.
// Returns the physical address, or -1 if not present / not user accessible.
int
sys_va2pa(void)
{
  int va;
  pte_t *pte;
  uint pa;

  if(argint(0, &va) < 0)
    return -1;
  pte = walkpgdir(myproc()->pgdir, (void*)va, 0);
  if(pte == 0 || !(*pte & PTE_P)){
    cprintf("va2pa: va=0x%x PDX=%d PTX=%d off=0x%x -> page not present\n",
            va, PDX(va), PTX(va), (uint)va & 0xFFF);
    return -1;
  }
  if(!(*pte & PTE_U)){
    cprintf("va2pa: va=0x%x PDX=%d PTX=%d off=0x%x flags P=%d W=%d U=0 -> not user accessible\n",
            va, PDX(va), PTX(va), (uint)va & 0xFFF, (*pte & PTE_P) != 0, (*pte & PTE_W) != 0);
    return -1;
  }
  pa = PTE_ADDR(*pte) | ((uint)va & 0xFFF);
  cprintf("va2pa: va=0x%x PDX=%d PTX=%d off=0x%x flags P=%d W=%d U=%d -> pa=0x%x\n",
          va, PDX(va), PTX(va), (uint)va & 0xFFF,
          (*pte & PTE_P) != 0, (*pte & PTE_W) != 0, (*pte & PTE_U) != 0, pa);
  return (int)pa;
}

// Q2: int getmeminfo(struct meminfo *mi)
int
sys_getmeminfo(void)
{
  struct meminfo *mi;
  struct meminfo k;
  struct proc *p = myproc();

  if(argptr(0, (char**)&mi, sizeof(*mi)) < 0)
    return -1;
  k.total_pages = ktotalpages();
  k.free_pages = kfreepages();
  k.user_pages = p->sz / PGSIZE;
  vmcount(p->pgdir, &k);
  memmove(mi, &k, sizeof(k));   // may fault a lazy page in; handled by trap()
  return 0;
}

// Q3: int vmprint(void)
int
sys_vmprint(void)
{
  vmdump(myproc()->pgdir, myproc()->sz);
  return 0;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
