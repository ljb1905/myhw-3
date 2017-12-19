#include <stdio.h>
#include <sys/types.h>
#include <limits.h>

#include "run.h"
#include "util.h"

void *base = 0;
void *end = 0;

p_meta find_meta(p_meta *last, size_t size) {
  p_meta index = base;
  p_meta result = base;
  if(base==last) return -1;
  switch(fit_flag){
    case FIRST_FIT:
    {
     while(index) 
      {
        if(result != base) break;
        if(index->free && index->size>=size) result = index;
        if(result->size > index->size) result = index;
          if(index->next == NULL) result = index;
        index = index->next;
      }
    }
    break;

    case BEST_FIT:
    {
      while(index)
      {
        if(result != base) break;
        if(index->free && index->size>=size)
          if(result->size > index->size) result = index;
        if(index->next == NULL) result = index;
        index = index->next;
      }
    }
    break;

    case WORST_FIT:
    {
      while(index)
      {
        if(result != base) break;
        if(index->free && index->size>=size)
          if(result->size < index->size) result = index;
        if(index->next == NULL) result = index;
        index = index->next;
      }
    }
    break;

  }
  return result;
}

void *m_malloc(size_t size) { 
if(base==0)
  {
    base = sbrk(0);
    end = base;
  }
  if(size%4!=0) size += (4-size%4);
  int l = size + META_SIZE;
  p_meta tmp = find_meta(end,size);
  p_meta tmp2 = end;
  end += l;
  int ret = brk(end);
  if(ret == -1) return ; 
  tmp2->free = 0;
  tmp2->next = 0;
  tmp2->prev = tmp;
  tmp2->size = size;
  if(tmp!=-1) tmp->next = tmp2;
  tmp = tmp2;
  return tmp->data;
}

void m_free(void *ptr) {
printf("ASdf");
}

void*
m_realloc(void* ptr, size_t size)
{
printf("Asdf");
}
