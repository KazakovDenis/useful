/*
gcc -shared -o clib.so clib.c
*/
#include <stdio.h>


int hello()
{
    printf("Hello from clib.c!");
}
