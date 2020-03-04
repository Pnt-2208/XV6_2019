// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"
#include "proc.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct spinlock common;

struct run {
  struct run *next;
};



struct {
  struct spinlock lock;
  struct run *freelist;
} kmem[8];




void
kinit()
{
 for (int i = 0; i < NCPU; i++){
    initlock(&kmem[i].lock, "kmem");
 }
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;
  
  push_off();
  int hart = cpuid();
  pop_off();


  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  acquire(&kmem[hart].lock);
  r->next = kmem[hart].freelist;
  kmem[hart].freelist = r;
  release(&kmem[hart].lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  
  struct run *r;
  int hart;
  int count = 0;  
  push_off();
  hart = cpuid();
  pop_off();

  acquire(&kmem[hart].lock);
  // 
  if(!kmem[hart].freelist){
    for(int i=0;i<8;++i){
      if(count==1)  
        break;
      if(i!=hart){
        if(kmem[i].freelist){
          for(int j=0;j<1-count;j++){
            if(!kmem[hart].freelist && kmem[i].freelist){
               acquire(&kmem[i].lock);
               kmem[hart].freelist= kmem[i].freelist;
               kmem[i].freelist = kmem[i].freelist->next;
               kmem[hart].freelist->next = 0;
               release(&kmem[i].lock);
               count++;
            } 
            else
              break;
          }

        }
      }
    }

  }

  //no page left in any cpu
  if(!kmem[hart].freelist){
   //printf("System ran out of memory\n");
    release(&kmem[hart].lock);
    return 0;
    
  }
  
  r = kmem[hart].freelist;
  if(r)
    kmem[hart].freelist = r->next;
  release(&kmem[hart].lock);

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;
}
