#include <iostream>
#include <cstring>
#include "sim_mem.h"

#define GREEN "\x1B[32m"

int main()
{
    char val;
    sim_mem mem_sm((char*)"exec_file.txt", (char*)"exec_file1.txt", (char*)"swap_file.txt" ,25, 50, 25,25, 25, 5,2);

    val = mem_sm.load(1,0);
    printf("%c" , val);
    val = mem_sm.load(1,2);
    printf("%c" , val);
    val = mem_sm.load(1,3);
    printf("%c" , val);
    mem_sm.print_memory();
    mem_sm.print_page_table();
    printf(GREEN "^^expected to be one file in the RAM!\n" WHITE);


    mem_sm.load(1 , 120);
    printf(GREEN "^^expected to failed because the memory not allocated!\n" WHITE);
    mem_sm.store(1,125,'x');
    mem_sm.store(1,-1,'x');
    printf(GREEN "^^expected to failed twice because address not valid!\n" WHITE);

    mem_sm.store(1,95,'x');
    mem_sm.store(1,90,'x');
    mem_sm.print_memory();
    mem_sm.print_page_table();
    printf(GREEN "^^creating two bss files starts with x total 3 in RAM\n" WHITE);


    mem_sm.store(1,96,'y');
    mem_sm.store(1,91,'y');
    mem_sm.print_memory();
    mem_sm.print_page_table();
    printf(GREEN "^^creating two bss files starts with x and than y total 3 in RAM\n" WHITE);

    for(int i = 25 ; i < 125 ; i++){
        mem_sm.store(1,i,'*');
    }
    for(int i = 25 ; i < 125 ; i++){
        mem_sm.store(2,i,'*');
    }
    mem_sm.print_memory();
    mem_sm.print_page_table();
    printf(GREEN "^^fill the ram with * .\n" WHITE);

    mem_sm.load(1,5);
    mem_sm.print_memory();
    mem_sm.print_page_table();
    mem_sm.print_swap();
    printf(GREEN "^^move the 5th page of process 1 to the SWAP * .\n" WHITE);

    mem_sm.print_memory();
    mem_sm.print_page_table();
    mem_sm.print_swap();
    printf(GREEN "^^return the 5th page of process 1 from swap: * .\n" WHITE);

}


