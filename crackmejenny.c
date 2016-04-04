/*
 * Title: Crackme Jenny
 * Author: Steven Lavoie, 2016
 * License: MIT (https://opensource.org/licenses/MIT)
 * Compiled with gcc version 4.9.2 (Debian 4.9.2-19):
 *  gcc -std=c11 -lm -ggdb3 -c crackme_jenny.c
 * References:
 *  GCC Asm Ref: https://www.ibiblio.org/gferg/ldp/GCC-Inline-Assembly-HOWTO.html
 *  Idea From: https://gist.github.com/anonymous/e486705dc16c3a687d3f
*/

#include <sys/mman.h> //for PROT_* constants
#include <unistd.h>   //for _SC_PAGESIZE
#include <stdio.h>
#include <string.h>   //for memcpy()
#include <math.h>

#define DEBUG

extern int add(int x, int y);
void mark_exe();

int main(int argc, char ** argv)
{
    if(argc < 2)
    {
        printf("Fail.\n");
        return 0;
    }

    mark_exe();

    char phone[7];
    memcpy(phone, &argv[1][6], 7); //=8675309
    int num = atoi(phone);
    
    int jennyxor = 0, phonexor = 0;

    for(int i = 0; i < 5; i++)
        jennyxor += argv[1][i] ^ num;

    for(int i = 5; i < 13; i++)
        phonexor += argv[1][i] ^ num;

#ifdef DEBUG
    printf("phonexor: %d\n", phonexor);
    printf("jennyxor: %d\n", jennyxor);
#endif
    argc <<= argc;
    
    if(jennyxor == 43376041 &&
        argv[1][(argc / (argc / 4)) + 1] == (argc << 2) * 2) //64=@
    {
        argc >>= 2;
#ifdef DEBUG
        printf("    argc: %u\n", argc);
#endif
        //imul = 0xcaaf0f //= 13,283,087d
        //0123456789012
        //jenny@8675309
        
        char twodig[argc];
        memcpy(twodig  , &argv[1][10], argc);   // = 30
        //printf("Twodig: %d\n", atoi(twodig));
        int opsub = (int)argv[1][5];   //@ = 64
        //printf("Opsub: %d\n", opsub);
        opsub -= atoi(twodig);          //64 - 30 = 34
        //printf("Opsub: %d\n", opsub);
        char seven = argv[1][(argc * argc * argc)];
        //printf("seven: %c\n", seven);
        int min = add((int)seven, -48);
        //printf("min: %d\n", min);
        opsub = add(opsub, min); // = 41
#ifdef DEBUG
        printf("  Opcode: 0x01 (add)\n");
#endif
        //int left = 5, right = 3;
        int left = opsub, right = 38;      //too lazy to change vars below
        int sum = add(left, right);
#ifdef DEBUG
        printf("Sum: %d + %d = %d\n", left, right, sum);
#endif
        //switch to subtraction
#ifdef DEBUG
        printf("********************************\n");
#endif
        int inst = opsub;     //sub = 41d
        memcpy(add+18, &inst, 1);
#ifdef DEBUG
        printf("  Opcode: 0x%x (sub)\n", inst);
#endif
        int diff = add(right, left);
        int xordiff = add(jennyxor, phonexor);
#ifdef DEBUG
        printf("    Diff: %d - %d = %d\n", left, right, diff);
        printf(" xordiff: %d - %d = %d\n", phonexor, jennyxor, xordiff);
#endif
        argc <<= 4;
#ifdef DEBUG
        printf("    argc: %u\n", argc);
#endif
        //argc = add(num, argc);
        //argc = 4286292019
        //argc factors are 79 * 293 * 185177
#ifdef DEBUG
        //printf("argc: %u\n", argc);
#endif
        //switch to multiplication
#ifdef DEBUG
        printf("********************************\n");
#endif
        //inst = 0xcaaf0f; //imul = 13283087d
        //0xca = 202, 0xaf = 175, 0x0f = 15, shl(8)
        //inst factors: 43 * 107 * 2887
        argc += 11;
        inst = argc * ((argc * 2) + 21) * 2887;
#ifdef DEBUG
        //printf("low_factor: %d\n", low_factor);
        printf("Opcode: 0x%x\n (mul)", inst);
#endif
        memcpy(add+17, &inst, 3);
        int prod = add(left, right);
#ifdef DEBUG
        printf("Product: %d * %d = %d\n", left, right, prod);
#endif
        printf("Your key: %d-", prod + inst);
        prod = add(prod, prod);
        printf("%d-", prod);
        prod = add(prod, argc);
        printf("%d\n", prod);
    }

    return 0;
}

void mark_exe()
{
    size_t pagesize = sysconf(_SC_PAGESIZE);
#ifdef DEBUG
    printf("********************************\n");
    printf("----------------------\n");
    printf("Page Size: %zd\n", pagesize);
#endif
    int add_addr = (unsigned int)&add;
#ifdef DEBUG
    printf(" Add Func: %08x\n", add_addr); 
#endif
    add_addr %= pagesize;
#ifdef DEBUG
    printf(" add\%4096: %d\n", add_addr);
#endif
    int ptr = (&add - add_addr);
#ifdef DEBUG
    printf("      Ptr: %d\n", ptr);
    printf("----------------------\n");
    printf("********************************\n");
#endif

    if(mprotect(ptr, pagesize, PROT_EXEC | PROT_WRITE | PROT_READ))
    {
#ifdef DEBUG
        perror("mprotect");
#endif
        printf("Fail.\n");
        exit(0);
    }
}

int add(int x, int y)
{
    //if 01, then add  (01 d1)      at add+18
    //if 24, then imul (0x0f af ca) at add + 17

    __asm__ volatile              //volatile = don't move/optimize this code
           ( "nop\n\t"            //just an easy way to identify these
             "nop\n\t"            //instructions in a debugger
             //"imull %0, %1\n\t"
             "addl %0, %1\n\t"
             "movl %1, %%eax\n\t"
             "nop\n\t"
             "nop\n\t"
           :                      //output vars (none)
           : "r"(x), "r"(y)       //input vars (x and y), put each in some(any) register
           : "%eax");             //tell GCC not to use eax since I screw it up
}
