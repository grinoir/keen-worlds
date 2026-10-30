// Synthetic Keen Dreams Gold companion: check its argv, write only its own save.
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
int main(int argc,char **argv) {
 if (getenv("GOLD_TEST_FAIL")) return 1;
 const char *fullscreen=getenv("GOLD_TEST_FULLSCREEN");assert(fullscreen);
 assert(argc==4);
 assert(!strcmp(argv[3],fullscreen));
 const char *data="/mods/dreams-gold", *saves="/dreams-gold";
 assert(!strcmp(argv[1]+strlen(argv[1])-strlen(data),data));
 assert(!strcmp(argv[2]+strlen(argv[2])-strlen(saves),saves));
 char path[2048];snprintf(path,sizeof(path),"%s/TEST.SAV",argv[2]);
 FILE *f=fopen(path,"wb");assert(f);fputs(argv[1],f);fclose(f);
 return 0;
}
