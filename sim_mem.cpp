// Ex5 - virtual Memory Management:
/**
 * The program simulates the memory management system in the computer.\n
 * this simulator will show how the memory brought to RAM from Disk/Swap , and how the memory move to the swap.\n
 * pay attention that this simulator is not save changes , the update files will move between swap and the RAM till the program finishes.\n

@author Itamar Brina, 208560920*/
#include "sim_mem.h"
char main_memory[MEMORY_SIZE];
using namespace std;

/**constructor: assuming that the program's files are exist:*/
sim_mem :: sim_mem (char exe_file_name1[],char exe_file_name2[], char swap_file_name[], int
        text_size, int data_size, int bss_size, int heap_stack_size, int num_of_pages, int
                 page_size, int num_of_process){
    /*init the regular variables:*/
    this->text_size = text_size;
    this->data_size = data_size;
    this->bss_size = bss_size;
    this->heap_stack_size = heap_stack_size;
    this->num_of_pages = num_of_pages;
    this->page_size = page_size;
    this->num_of_proc = num_of_process;
    this->frame_index = -1;


    /*checking if the num of pages is zero:*/
    if (num_of_pages <= 0){
        printf(RED "The number of pages can not be zero!\n" WHITE);
        exit(1);
    }

    /*creating the pages table:*/
    pages_table_creator();

    /*clearing the 'ram':*/
    for(char & i : main_memory)
        i = '0';

    /*opening the program's files:*/
    program_files_opener(exe_file_name1 , exe_file_name2 ,swap_file_name);

    /*defining variable that holds if the frame is empty or not:*/
    frame_cell = EMPTY;

    /*defining swap array:*/
    this->swaps = (bool*) malloc ((num_of_pages - (text_size / page_size))* num_of_proc * sizeof (bool));
    if (this->swaps == nullptr)
        error("MEMORY ALLOCATION FAILED!\n");
    /*initialize the swaps array:*/
    for (int i = 0; i < (num_of_pages - (text_size / page_size))* num_of_proc ; i++)
        swaps[i] = EMPTY;
}


/**This method printing the RAM content.*/
void sim_mem::print_memory() {
    int i;
    printf("\n Physical memory\n");
    for(i = 0; i < MEMORY_SIZE; i++) {
        printf("[%c]\n", main_memory[i]);
    }
}

/**This method printing the swap content.*/
void sim_mem::print_swap() {
    char * str = (char *) malloc(this->page_size *sizeof(char));
    int i;
    printf("\n Swap memory\n");
    lseek(swapfile_fd, 0, SEEK_SET); // go to the start of the file
    while(read(swapfile_fd, str, this->page_size) == this->page_size) {
        for(i = 0; i < page_size; i++) {
            printf("%d - [%c]\t", i, str[i]);
        }
        printf("\n");
    }
    free(str);
}

/**This method prints the page tables.*/
void sim_mem::print_page_table() {
    int i;
    for (int j = 0; j < num_of_proc; j++) {
        printf("\n page table of process: %d \n", j);
        printf("Valid\t Dirty\t Permission \t Frame\t Swap index\n");
        for(i = 0; i < num_of_pages; i++) {
            printf("[%d]\t[%d]\t[%d]\t[%d]\t[%d]\n",
                   page_table[j][i].V,
                   page_table[j][i].D,
                   page_table[j][i].P,
                   page_table[j][i].frame ,
                   page_table[j][i].swap_index);
        }
    }
}

/**This function simulating how the computer loading data from the disk.\n
 * @param process_id the number of the process.
 * @param address is the physical address on the disk of the data.
 * @return The function return the certain data (char) in the address .\n
 * note:that the function first moving the page to the 'RAM' and than loads
 * */
char sim_mem::load(int process_id, int address){
    /*fix the process id:*/
    process_id--;

    /*checking the process id:*/
    if((num_of_proc == 1 && process_id != 0) || (num_of_proc == 2 && process_id != 0 && process_id != 1)){
        printf(RED "The process id does not exist!\n" WHITE);
        return '\0';
    }

    /*checking if the address is legal:*/
    if (address >= (num_of_pages * page_size)|| address < 0){
        printf(RED "The address is not valid!!\n" WHITE);
        return '\0';
    }

    /*define two temporary variables that help to calculate the physic location:*/
    int page_num = (int) address / page_size;
    int offset = address % page_size;

    /*checking if the page is already loaded to the RAM:*/
    if(page_table[process_id][page_num].V == 1)
        return main_memory[((page_table[process_id][page_num].frame) * page_size) + (offset)];


    /*checking if the file is in the swap:*/
    if (page_table[process_id][page_num].P == 1){
        /*checking if the page is in the swap file:*/
        if(page_table[process_id][page_num].D == 1) {
            fetch_from_swap(&page_table[process_id][page_num].swap_index, &process_id, &page_num);
            return main_memory[(frame_index * page_size) + offset];
        }

        /*checking if the file is belongs to heap/stack:*/
        if(page_num >= (text_size + data_size + bss_size) / page_size){
                printf(RED "sigsegv - page %d doesn't exist.\n" WHITE , page_num);
                return '\0';
            }

        /*checking if the page is belongs to bss:*/
        if (page_num < (text_size + data_size + bss_size) / page_size && page_num >= (text_size + data_size) / page_size) {
            create_new_page(&process_id, &page_num);
            return main_memory[frame_index * page_size + offset];
        }
    }

    /*write the page to the RAM:*/
    fetch_to_RAM(&process_id , &page_num);

    return main_memory[(frame_index*page_size) + offset];
}

/**This function simulating how the computer store data from the disk.\n
 * @param process_id the number of the process.
 * @param address is the physical address on the disk that the data will be in.
 * @param value is the value we want to store.
 * @return The function return the certain data (char) in the address .\n
 * note:that the function first moving the page to the 'RAM' and than store.
 * */
void sim_mem::store(int process_id, int address, char value) {
    /*fix the process id:*/
    process_id--;

    /*checking the process id:*/
    if((num_of_proc == 1 && process_id != 0) || (num_of_proc == 2 && process_id != 0 && process_id != 1)){
        printf(RED "The process id does not exist!\n" WHITE);
        return;
    }

    /*checking if the address is legal:*/
    if (address >= (num_of_pages * page_size) || address < 0){
        printf(RED "The address is not valid!!\n" WHITE);
        return;
    }

    /*define two temporary variables that help to calculate the physic location:*/
    int page_num = (int) address / page_size;
    int offset = address % page_size;

    /*checking if the file is text file:*/
    if (page_table[process_id][page_num].P == 0) {
        printf(RED "The page %d doesn't have write permissions!\n" WHITE , page_num);
        return;
    }

    /*checking if the page is already loaded to the RAM:*/
    if(page_table[process_id][page_num].V == 1) {
        main_memory[(page_table[process_id][page_num].frame * page_size) + (offset)] = value;
        page_table[process_id][page_num].D = 1;
        return;
    }


        /*checking if the page is in the swap file:*/
        if (page_table[process_id][page_num].D == 1) {
            fetch_from_swap(&page_table[process_id][page_num].swap_index, &process_id, &page_num);
            main_memory[(frame_index * page_size) + offset] = value;
            // -> attention here frame_index holds the last frame that was in use, so we can use him directly!!!!
            page_table[process_id][page_num].V = 1;
            return;
        }

        /*checking if the page is belongs to bss:*/
        if (page_num >= (text_size + data_size) / page_size) {
            create_new_page(&process_id, &page_num);
            main_memory[frame_index * page_size + offset] = value;
            // -> attention here frame_index holds the last frame that was in use, so we can use him directly!!!!
            page_table[process_id][page_num].D = 1;
            return;
        }
        /*if we get to here that mean that the page is DATA page and its in the executable:*/
    /*write the page to the RAM:*/
    fetch_to_RAM(&process_id , &page_num);

     main_memory[(frame_index*page_size) + offset] = value;
    // -> attention here frame_index holds the last frame that was in use, so we can use him directly!!!!
    page_table[process_id][page_num].D = 1;
}

/**This function defines and allocate the pages table array
 * dynamically for each process.*/
void sim_mem::pages_table_creator(){

    /*allocating the table's pointer dynamically:*/
    this->page_table = (page_descriptor**) malloc(num_of_proc * sizeof(page_descriptor *));
    if(this->page_table == nullptr)
        error("MEMORY ALLOCATION FAILED!\n");

    /*creating the content of the array dynamically:*/
    for (int i = 0 ; i < num_of_proc ; i++) {
        page_table[i] = (page_descriptor *) malloc(num_of_pages * sizeof(page_descriptor));
        if (*(page_table) == nullptr)
            error("MEMORY ALLOCATION FAILED!\n");

        /*initialize the content:*/
        for (int j = 0; j < num_of_pages; j++) {
            ((page_table)[i])[j].D = 0;

            if (j < text_size / page_size)
                ((page_table)[i])[j].P = 0;
            else
                ((page_table)[i])[j].P = 1;

            ((page_table)[i])[j].V = 0;
            ((page_table)[i])[j].frame = -1;
            ((page_table)[i])[j].swap_index = -1;
        }
    }
}

/**This function prints system errors , the use of this function is only to ease the reading
 * and make the writing more comfortable.*/
void sim_mem ::error (const char* massage){
    perror(massage);
    exit(1);
}

/**This function aims to open the files that the user use. \n
 * note that the exe_file should be exist before the running.
 * @param exe_file_name1 holds the name of the exe file.
 * @param exe_file_name2 hold the name of the second exe file (in 2 processes case).
 * @param swap_file_name is the name of the swap file*/
void sim_mem ::program_files_opener(char * exe_file_name1 , char * exe_file_name2, char * swap_file_name){
    /*opening the programs files:*/
    if (exe_file_name1 == nullptr){
        printf(RED "Got empty name!\n" WHITE);
        exit(1);
    }

    program_fd[0] = open(exe_file_name1 , O_RDWR);
    if(program_fd[0] < 0)
        error("cannot open file!\n");

    /*opening the swap file:*/
    if (swap_file_name == nullptr){
        printf(RED "Got empty name!\n" WHITE);
        exit(1);
    }
    swapfile_fd = open(swap_file_name , O_RDWR|O_CREAT , 0666);
    if(swapfile_fd < 0)
        error("cannot create file!\n");

    for (int i = 0 ; i < (data_size+bss_size+heap_stack_size) * num_of_proc ; i++)
        if (write(swapfile_fd , "0" , 1) < 0)
            error("err_writing to file!\n");

    if (num_of_proc == 2) {
        if (exe_file_name2 == nullptr){
            printf(RED "Got empty name!\n" WHITE);
            exit(1);
        }
        this->program_fd[1] = open(exe_file_name2, O_RDWR);
        if(this->program_fd[1] < 0)
            error("cannot open file!\n");
    }
}

/**This function fetching certain page to the RAM.
 * @param process_id pointer to the variable that holds the number of the process.
 * @param page_num pointer to variable that holds the number of the page that will be fetch.
 * @result In the end of this function the result will be that the page is loaded into teh frame in the RAM*/
void sim_mem::fetch_to_RAM(const int* process_id ,const int* page_num ){
    available_frame();
    /*fetch the page to the RAM:*/
    lseek( program_fd[*process_id] , *page_num * page_size , SEEK_SET);
    read(program_fd[*process_id] , main_memory+(frame_index * page_size), page_size);

    /*after the fetching:*/
    page_table[*process_id][*page_num].V = 1;
    page_table[*process_id][*page_num].frame = frame_index;

    /*push the page num to the RAM queue*/
    RAM_queue.push(*page_num + (num_of_pages * (*process_id)));

}

/**This function fetching page from the swap to the RAM
 * @param swap_index pointer to the variable that holds the swap index
 * @param process_id pointer that holds the process num.
 * @param page_num pointer that holds the number of page we want to fetch.*/
void sim_mem::fetch_from_swap( const int* swap_index , const int* process_id , const int* page_num) {
    /*generate free frame into frame_index:*/
    available_frame();
    /*fetch the page to the RAM:*/
    lseek(swapfile_fd , *swap_index * page_size , SEEK_SET);
    read(swapfile_fd , main_memory+(frame_index * page_size), page_size);

    /*fill the page with zeroes:*/
    lseek(swapfile_fd , *swap_index * page_size , SEEK_SET);
    for(int i = 0 ; i < page_size ; i++)
        write(swapfile_fd , "0" , 1);

    /*after the fetching:*/
    page_table[*process_id][*page_num].V = 1;
    page_table[*process_id][*page_num].frame = frame_index;
    swaps[*swap_index] = EMPTY;
    page_table[*process_id][*page_num].swap_index = -1;
    /*push the page num to the RAM queue*/
    RAM_queue.push(*page_num + (num_of_pages * (*process_id)));
}

/**This function creating new pages.
 * @param process_id holds pointer to the variable that holds the process id.
 * @param page_num pointer to var that holds the page number in the physic memory.
 * @result The function creating a page full with zeros.*/
void sim_mem::create_new_page(const int * process_id,const int *page_num ) {
    available_frame();
    /*full the page with zeroes (the changes will be in the RAM here)*/
    for (int i = 0 ; i < page_size ; i++)
        main_memory[frame_index * page_size + i] = '0';
    /*update the page table's fields:*/
    page_table[*process_id][*page_num].frame = frame_index;
    page_table[*process_id][*page_num].V = 1;
    /*adding to RAM's queue:*/
    RAM_queue.push(*page_num + (num_of_pages * (*process_id)));
}

/**This function move page from RAM to SWAP or delete organ when the RAM is already full.
 * @result global var frame_index will hold the available frame in the RAM that cleared.*/
void sim_mem::delete_from_RAM() {
    /*first need to pop out the first element from the RAM:*/
    int deleted_page = RAM_queue.front();
    /*deleting the first organ in the queue:*/
    RAM_queue.pop();
    /*change the frame to be the cleared frame.*/
    frame_index = page_table[deleted_page/num_of_pages][deleted_page % num_of_pages].frame;

    /*copy the page to the swap if it is dirty:*/
    if(page_table[deleted_page/num_of_pages][deleted_page % num_of_pages].D==1){
    /*find clear space in the swaps array:*/
    int swap_ind = 0;
    while(swaps[swap_ind] != EMPTY)
        swap_ind ++;
    swaps[swap_ind] = FULL;
    lseek(swapfile_fd , swap_ind * page_size , SEEK_SET);
    if(write(swapfile_fd , main_memory + frame_index * page_size , page_size) < 0)
        error("cannot write to file!\n");
    page_table[deleted_page/num_of_pages][deleted_page % num_of_pages].swap_index = swap_ind;
    }

    page_table[deleted_page/num_of_pages][deleted_page % num_of_pages].V = 0;
    page_table[deleted_page/num_of_pages][deleted_page % num_of_pages].frame = -1;
}

/**This method generate available frame in the RAM.
 * @result global var frame_index will hold the available frame.*/
void sim_mem::available_frame() {
    frame_index++;
    if (frame_index >= MEMORY_SIZE/page_size){
        frame_index = 0;
        frame_cell = FULL;
    }
    if(frame_cell == FULL)
        delete_from_RAM();
}

/**The destructor function is responsible to free all the allocated memory and
 * close all the files that the program used.*/
sim_mem :: ~sim_mem(){

    /*closing the program files:*/
    close(this->program_fd[0]);
    if(this->num_of_proc == 2)
        close(this->program_fd[1]);

    /*closing the swap file:*/
    close(this->swapfile_fd);

    /*free the table's array:*/
    free(this->page_table[0]);
    if (this->num_of_proc == 2)
        free(this->page_table[1]);
    free(this->page_table);

    /*free the swaps array:*/
    free(this->swaps);
}
