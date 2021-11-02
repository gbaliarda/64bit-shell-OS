// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "test_util.h"
#include "../syscalls.h"
#include "../libc.h"
#include "tests.h"

#define MAX_BLOCKS 128
#define MAX_MEMORY (134217728*0.8) //Should be around 80% of memory managed by the MM

typedef struct MM_rq{
  void *address;
  uint32_t size;
}mm_rq;

static void * memset(void * destination, int32_t c, uint64_t length)
{
	uint8_t chr = (uint8_t)c;
	char * dst = (char*)destination;

	while(length--)
		dst[length] = chr;

	return destination;
}

void test_mm(){
  mm_rq mm_rqs[MAX_BLOCKS];
  uint8_t rq;
  uint32_t total;
  uint32_t j = 1;
  while (j < 20){
    rq = 0;
    total = 0;
    // Request as many blocks as we can
    while(rq < MAX_BLOCKS && total < MAX_MEMORY){
      mm_rqs[rq].size = (rq+100)*(rq+1)*j*2;
      mm_rqs[rq].address = sys_alloc(mm_rqs[rq].size); // TODO: Port this call as required
      if(mm_rqs[rq].address != NULL)
        total += mm_rqs[rq].size;
      rq++;
    }


    // Set
    uint32_t i;
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address != NULL) {
        memset(mm_rqs[i].address, i, mm_rqs[i].size); // TODO: Port this call as required
      }


    // Check
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address != NULL)
        if(!memcheck(mm_rqs[i].address, i, mm_rqs[i].size)) {
          printf("ERROR!\n"); // TODO: Port this call as required
        }

    // Free
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address != NULL)
        sys_free(mm_rqs[i].address);  // TODO: Port this call as required
  

    j++;
    printf("Succeed\n");
  }
}