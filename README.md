# VirtualMemoryManagement
Illustration of how the computer's memory management system works.<br>
This program writes as part of the operating system course at Azrieli College.<br>

## Description

The program gives an illustration of how the computer's memory management system works.<br>
The program does it by getting a random load and storing commands from the user and performing a part of the management system tasks:<br>
<ol>
	<li> transferring pages from EXECUTABLE to RAM.</li>
	<li> transferring pages from RAM to SWAP.</li>
	<li> transferring pages from SWAP to RAM. </li>
</ol>

#### Note that :
<ol>
	<li> The program doesn't transfer real commands like in the computer, it moves characters instead.</li>
	<li> The updates that the user does in running time will not be saved to the EXECUTABLE files, which means that the updated file can 
		be either in the RAM or in the SWAP all the time, and nowhere else.</li>
</ol>

#### In this program, we use:
<ol>
	<li> an array of chars in size 200 (main_memory), that array represents the RAM.</li>
	<li> struct page_descriptor containing the following fields:
    <ol>
		   <li> int V - the bit that shows if the file is in the RAM.</li>
		   <li> int D - the bit that shows if the file is updated.</li>
		   <li> int frame - holds the number of the frame in the RAM that the file is in, or -1 if D = 0.</li>
		   <li> int swap_index - holds the position that the file is in the swap file or -1 if it is not there.</li>
		   <li> int P - the bit that shows if the file has only read permissions (P=0), or read and write permissions (P=1).</li>
    </ol>
  </li>
	<li> array of type `page_descriptor` for every process (up to 2) .(the size of the array is like the size of the pages in the process).</li>
	<li> swaps - an array of booleans that show the available cells in the swap file.</li>
	<li> frame_cell - variable type bool that shows if the place in the RAM we want to use is empty or full.</li>
	<li> frame_index - a variable that holds the frame in the RAM that the fetched page will be in.</li>
	<li> RAM_queue - a queue of the pages that are loaded to the RAM.</li>
</ol>

## functions:

There are four main functions:
  <ul>
 	  <li>mem sim_mem :: sim_mem (char exe_file_name1[],char exe_file_name2[], char swap_file_name[], int text_size, int data_size, int bss_size, 
		int heap_stack_size, int num_of_pages, int page_size, int num_of_process)<br>
		- The constructor of sim_mem, in charge of initializing all the variables, allocating memory, and opening files:
      <ol>
			  <li> exe_file_name1 will hold the name of the first executable file - the file should have existed before the running.</li>
			  <li> exe_file_name2 will hold the name of the second executable file - the file should have existed before running if there are 2 processes.</li>
			  <li> swap_file_name will hold the name of the swap file that the program will create and fill with zeroes in the size of all the pages that could 
						enter to the swap ( all the pages except pages with permission 0).</li>
			  <li> text_size - holds the amount of the chars that belong to TEXT pages. (note that this number should be a number that can be divided by page_size)</li>
			  <li> data_size - holds the amount of the chars that belong to DATA pages. (note that this number should be a number that can be divided by page_size)</li>
			  <li> bss_size - holds the amount of the chars that belong to BSS pages. (note that this number should be a number that can be divided by page_size)</li>
			  <li> heap_stack_size - holds the amount of the chars that belong to HEAP\STACK pages. (note that this number should be a number that can be divided by page_size)</li>
			  <li> num_of_pages -contains the num of pages of all the pages in the process.</li>
			  <li> page_size - holds the size of chars on one page.</li>
			  <li> num_of_process) - holds the number of the process the program except (up to two processes).</li>
      </ol>
    </li>
	<li> char sim_mem::load(int process_id, int address)- this function is used to load data after this function has been called by the user  on the page that contains the 
				certain data (the data in the address that is given) will be in the RAM, the function returns the certain data.</li>
	<li> void sim_mem::store(int process_id, int address, char value) -  this function is used to store data after this function has been called by the user on the page that contains the certain data the user wants to change (the data in the address that is given) will be in the RAM, the function put the char in the certain address.</li>
	<li> sim_mem:: ~sim_mem() - The destructor of sim_mem, takes care of freeing the allocated memory and closing all the files.</li>
  </ul>

In addition, there are several more helping functions:
<ul>
	<li> void sim_mem::print_memory() - this function is used to display the RAM to the user.</li>
	<li> void sim_mem::print_swap() - this function is used to display the SWAP to the user.</li>
	<li> void sim_mem::print_page_table() - this function used to display the page_table to the user.</li>
	<li> void sim_mem::pages_table_creator() - this function helps the constructor to allocate the page table arrays.</li>
	<li> void sim_mem ::error (const char* massage) - This function prints system errors.</li>
	<li> void sim_mem::program_files_opener(char * exe_file_name1, char * exe_file_name2, char * swap_file_name) - helps the constructor to open the files that the program using.</li>
	<li> void sim_mem::fetch_to_RAM(const int* process_id ,const int* page_num ) - This function fetching certain page to the RAM.</li>
	<li> void sim_mem::fetch_from_swap( const int* swap_index, const int* process_id, const int* page_num) - This function fetches the page from the swap to the RAM.</li>
	<li> void sim_mem::create_new_page(const int * process_id,const int *page_num ) -  This function creating new pages.</li>
	<li> void sim_mem::delete_from_RAM() - This function move page from RAM to SWAP or delete organ when the RAM is already full.</li>
	<li> void sim_mem::available_frame() - This method generates available frames in the RAM.</li>
</ul>

## Program files

The program itself is divided into the following files:  "sim_mem.cpp", and "sim_mem.h".<br>
The program uses external files:
<ul>
	<li> exe_file_name1 - the first process's executable file.</li>
	<li> exe_file_name2 - the second process's executable file.</li>
	<li> swap_file_name - the swap file that contains deleted updated files.</li>
</ul>

==How to compile?==
two options are possible:
at first, the user needs to create a main file (the main.cpp is an example of that file) that includes sim_mem.h library, then:
1.compile through clion compiler (or other c editor) and use through the console.
2. compile through the terminal using g++ sim_mem.cpp <main_name>.cpp -o main.
   and then: ./main.
