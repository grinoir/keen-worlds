// A guarded compressed byte must never be read beyond its declared length.
#define _GNU_SOURCE
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
typedef struct { uint16_t bit_0, bit_1; } ca_huffnode;
void CAL_HuffExpand(void *, void *, int, ca_huffnode *, int);
int main(void) {
 long page=sysconf(_SC_PAGESIZE);
 uint8_t *memory=mmap(NULL,page*2,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);
 assert(memory!=MAP_FAILED);assert(!mprotect(memory+page,page,PROT_NONE));
 uint8_t *source=memory+page-1;*source=0xaa;
 ca_huffnode table[255]={{0}};table[254].bit_0='A';table[254].bit_1='B';
 uint8_t out[16];memset(out,0xcc,sizeof(out));
 CAL_HuffExpand(source,out,16,table,1);
 assert(!memcmp(out,"ABABABAB",8));
 for (int i=8;i<16;++i)assert(out[i]==0xcc);
 CAL_HuffExpand(memory+page,out,16,table,0);
 CAL_HuffExpand(memory+page,out,0,table,0);
 munmap(memory,page*2);
 puts("PASS: Huffman compressed-data boundary and empty inputs, with a protected adjacent page");return 0;
}
