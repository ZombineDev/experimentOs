
/* Usermode application */

#include <commonDefs.h>
#include "usermode_syscall_wrappers.h"
#include "string.h"

void main()
{
   int stackVar;

   for (int i = 0; i < 10; ++i) {
      asmVolatile("" : : : "memory");
   }

   printf("Hello from init!\n");
   printf("&stackVar = %p\n", &stackVar);

magic_debug_break();
   int ret = open("/myfile.txt", 0xAABB, 0x112233);
magic_debug_break();
   printf("ret = %i\n", ret);
magic_debug_break();
   for (int i = 0; i < 5; i++) {
      printf("i = %i\n", i);
   }

   while (1);
}

