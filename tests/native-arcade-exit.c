// Exercise the real shared exit path used by original Keen 1, 2 and 3.
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
static int phase, error;
void CVort_save_ctrls(void) { assert(!phase && !error);phase=1; }
void CVort_engine_shutdown(void) { assert(phase==(error?0:1));phase=2; }
static void checked(void) { assert(phase==2);puts("PASS: native arcade exits without a DOS screen or extra key wait"); }
void CVort_chg_vid_and_error(const char *message);
int main(int argc,char **argv)
{
 (void)argv;error=argc>1;
 assert(!atexit(checked));
 CVort_chg_vid_and_error(error?"test error":"");
 return 99;
}
