#include "process.h"

extern Heap g_kernelHeap;
extern page_directory* g_page_directory;

int g_highest_pid = 1;
process_context_block* g_curr_process; 

list* g_process_list;
list* g_ready_processes_list;

process_context_block* create_process(int is_kernel, char* process_name);
int generate_pid();
void initialize_process_regs(process_context_block* pcb);
void load_process_code(process_context_block* pcb, char* file_name);
void kill_process(process_context_block* pcb);
void process_scheduler(registers_t* registers);
void process_init();
int context_switch(process_context_block* next_process);
void save_registers(process_context_block* pcb, registers_t* registers);


process_context_block* create_process(int is_kernel, char* process_name)
{
    process_context_block* pcb = (process_context_block*)heap_malloc(&g_kernelHeap, sizeof(process_context_block));

    pcb->is_kernel = is_kernel;
    pcb->pid = generate_pid();
    memcpy(pcb->name, process_name, PROCESS_NAME);
    initialize_process_regs(pcb);

    //alloc process page directory
    pcb->curr_page_directory = (page_directory*)kmalloc(sizeof(page_directory));
    memcpy(pcb->curr_page_directory, g_page_directory, sizeof(page_directory));

    //mapping stack
    pcb->process_pages[1] = page_to_address(page_alloc());
    pcb->process_pages[0] = page_to_address(page_alloc());
    pcb->stack_base = pcb->process_pages[0];

    // allocating apace for heap
    for(int i =2; i < 5; i ++)
    {
        pcb->process_pages[i] = page_to_address(page_alloc());
    }
    
    // crating heap object 
    heap_init(&(pcb->process_heap), pcb->process_pages[2], PAGE_SIZE * 3);


    load_process_code(pcb, process_name);

    pcb->process_state = PROCESS_READY;

    // adding the process to the processes list
    insert_head(g_process_list, pcb);
    insert_head(g_ready_processes_list, pcb);

    return pcb;
}

int generate_pid()
{
    // if there is no more space for more processes 
    if(g_highest_pid == MAX_PROCESS)
    {
        return NULL;
    }

    // inc the pid counter
    g_highest_pid++;

    return g_highest_pid - 1;
}

void initialize_process_regs(process_context_block* pcb)
{
    pcb->reg.eax = 0;
    pcb->reg.ebx = 0;
    pcb->reg.ecx = 0;
    pcb->reg.edx = 0;
    pcb->reg.esi = 0;
    pcb->reg.edi = 0;
    pcb->reg.eflags = 518;
}

void load_process_code(process_context_block* pcb, char* file_name)
{
    // TODO
    // will be implemented when elf end file system will be implemented 
    // TODO
}

void kill_process(process_context_block* pcb)
{

    // free stack and heap
    for(int i = 0; i < 5; i++)
    {
        page_free(address_to_page(pcb->process_pages[i]));
        memset((void*)page_to_address(pcb->process_pages[i]), 0, PAGE_SIZE);
    }

    heap_free(&(g_kernelHeap), pcb);
}

void process_scheduler(registers_t* registers)
{
    process_context_block* next_process;
    // if no running processes 
    if(g_ready_processes_list->size == NULL)
    {
        puts("Error - no running process");
        return;
    }

    // going through the ready processes list and getting ready process
    while(next_process->process_state == PROCESS_READY && next_process!= NULL)
    {
        next_process = (process_context_block*)pop_tail(g_ready_processes_list)->data;
    }

    // saving the old process registers
    save_registers(g_curr_process, registers);

    // pushing the current process to the list head
    insert_head(g_ready_processes_list, g_curr_process);
    g_curr_process->process_state = PROCESS_READY;

    // context switching to the next process 
    context_switch(next_process);


}

void process_init()
{
    //initilizing the processes lists 
    g_process_list = create_list();
    g_ready_processes_list = create_list();

    // initializing the scheduler
    set_scheduler((callback_function)process_scheduler);

    // initializing the first process idle
    process_context_block* idle = create_process(0, "idle.elf");
    idle->process_state = PROCESS_ZOMBI;
    insert_head(g_ready_processes_list, idle);
    insert_head(g_process_list, idle);
}

int context_switch(process_context_block* next_process)
{
    next_process->process_state = PROCESS_RUNNING;
    for (int i = 0; i < MAX_PROCESS_PAGES; i++)
    {
        // unmaping the old process meory
        page_unmap(g_curr_process->process_pages[i]);

        // mapping the new process memory
        page_map(next_process->curr_page_directory, next_process->process_pages[i], next_process->process_pages[i], PAGE_FLAG_READWRITE | PAGE_FLAG_USER);
    }


    // return to usermode 
    return_to_usermode(next_process->reg);
    int x = 0; 
}

void save_registers(process_context_block* pcb, registers_t* registers)
{
    // copying the updated registers vlues into the process pcb
    memcpy(&(pcb->reg), registers, sizeof(registers));
}