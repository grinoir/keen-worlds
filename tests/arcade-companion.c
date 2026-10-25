// Synthetic native companion: verify exact argv and write only its own save.
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
int main(int argc,char **argv) {
 const char *fullscreen=getenv("ARCADE_TEST_FULLSCREEN");assert(fullscreen);
 assert(argc==5);
 assert(!strcmp(argv[4],fullscreen));
 int n=argv[3][strlen(argv[3])-1]-'0';assert(n>=1 && n<=3);
 char suffix[32];snprintf(suffix,sizeof(suffix),"/keen%d",n);
 assert(!strcmp(argv[1]+strlen(argv[1])-strlen(suffix),suffix));
 assert(!strcmp(argv[2]+strlen(argv[2])-strlen(suffix),suffix));
 char path[2048];snprintf(path,sizeof(path),"%s/TEST.SAV",argv[2]);
 FILE *f=fopen(path,"wb");assert(f);fputs(argv[1],f);fclose(f);
 return 0;
}
