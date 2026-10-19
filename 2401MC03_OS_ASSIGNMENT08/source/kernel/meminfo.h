// Q2: statistics returned by getmeminfo().  Shared by kernel and user code.
struct meminfo {
  int total_pages;     // physical pages managed by kalloc (at boot)
  int free_pages;      // pages currently on the kalloc free list
  int user_pages;      // sz / PGSIZE  (size of the user address space in pages)
  int pt_pages;        // ALL pages used by this process's page tables
                       //   (page directory + every second-level table, incl. kernel half)
  int pt_user_pages;   // second-level tables covering the user half only
  int resident_pages;  // user pages that really have a physical frame (P bit set)
};
