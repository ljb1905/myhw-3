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
  if(tmp==-1 || (!tmp->next && (!tmp->free || tmp->free && tmp->size<size))){
  //empty handling
  p_meta tmp2 = end;
  end += l;
  int ret = brk(end);
  if(ret == -1) return ;
  tmp2->prev = tmp;
  tmp2->size = size;
  if(tmp!=-1) tmp->next = tmp2;
  tmp = tmp2;
}
else m_realloc(tmp->data,size);
  return tmp->data;
}

void m_free(void *ptr) {
  p_meta now = ptr-META_SIZE;
  now->free = 1;
  if(now->next && now->next->free ==1) // next O
  {
    now->size = now->size+now->next->size + META_SIZE;
    now->next = now->next->next;
  }
  if(now->prev!=-1 && now->prev->free ==1) // pre O
  {
    now = now ->prev;
    now->size = now->size+now->next->size + META_SIZE;
    now->next = now->next->next;
  }
  if(now->prev!=-1 && now->next==NULL)
  {
    end = end - now->size + META_SIZE;
    now->prev->next = 0;
  }
  if(now->next==NULL && now->prev==NULL) end = base;
  ptr=0;
}

void*
m_realloc(void* ptr, size_t size)
{
  p_meta now = ptr-META_SIZE;
  int flag = 0;
  if(size%4!=0) size += (4-size%4);
  if(now->size == size) return ptr; // don't need to realloc
  if(now->size < size){
    //size_t chk = now->size + now->next->size;
    if(now->next && now->next->free && now->size+now->next->size + META_SIZE>=size){

      now->size = now->size + now->next->size + META_SIZE;
      now->next = now->next->next;
      now->next->prev = now;
      if(now->size-size < META_SIZE) // don't need to realloc
        return ptr;
      else
        flag = 1;
    }
    else {
      m_free(now->data);
      void * start = m_malloc(size);
      strcpy(start, ptr); 
      return start;
    }
  }
  else if(flag==0 && now->size-size < META_SIZE)
    return ptr;
  else
   flag = 1;
  if(flag)
  {
    p_meta tmp = (int)now + size + META_SIZE;
    tmp->size = now->size - size - META_SIZE;
    tmp->prev = now;
    tmp->next = now->next;
    now->size = size;
    now->free = 0;
    now->next = tmp;
    m_free(tmp->data);
    return now->data;
  }
}
