// Ex5 - virtual Memory Management:
/**
 * The program simulates the memory management system in the computer.\n
 * this simulator will show how the memory brought to RAM from Disk/Swap , and how the memory move to the swap.\n
 * pay attention that this simulator is not save changes , the update files will move between swap and the RAM till the program finishes.\n

@author Itamar Brina, 208560920*/

#ifndef EX5_SIM_MEM_H
#define EX5_SIM_MEM_H
#include <iostream>
#include <unistd.h>
#include <string>
#include <fcntl.h>
#include <queue>

#define MEMORY_SIZE 200
#define RED "\x1B[31m"
#define WHITE "\x1B[0m"
#define FULL true
#define EMPTY false

extern char main_memory[MEMORY_SIZE];

using namespace std;

typedef struct page_descriptor
{
    int V; // valid
    int D; // dirty
    int P; // permission
    int frame; //the number of a frame if in case it is page-mapped
    int swap_index; // where the page is located in the swap file.
} page_descriptor;

class sim_mem {
    int swapfile_fd{}; //swap file fd
    int program_fd[2]{}; //executable file fd
    int text_size;
    int data_size;
    int bss_size;
    int heap_stack_size;
    int num_of_pages;
    int page_size;
    int num_of_proc;
    page_descriptor **page_table{}; //pointer to page table
    bool frame_cell; //pointer to array that show if there is available frames in the RAM
    int frame_index;
    bool * swaps; // pointer to array that show if there is available frames in the RAM
    queue <int> RAM_queue; //queue to the Ram organs.
private:
    void pages_table_creator();
    void error (const char* massage);
    void program_files_opener(char * exe_file_name1 , char * exe_file_name2 , char * swap_file_name);
    void delete_from_RAM();
    void fetch_to_RAM(const int* process_id ,const int* page_num);
    void fetch_from_swap(const int* swap_index, const int* process_id , const int* page_num);
    void create_new_page(const int * process_id ,const int* page_num);
    void available_frame();
public:
    sim_mem(char exe_file_name1[],char exe_file_name2[], char swap_file_name[], int
    text_size, int data_size, int bss_size, int heap_stack_size, int num_of_pages, int
            page_size, int num_of_process);

    ~sim_mem();

    char load(int process_id, int address);

    void store(int process_id, int address, char value);

    void print_memory();

    void print_swap();

    void print_page_table();
};

#endif //EX5_SIM_MEM_H
