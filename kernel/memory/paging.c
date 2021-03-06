#include "paging.h"
#define NULL 0

//array of bytes each bit represents page
uint8_t g_pages_array[PAGES_COUNT];

page_directory* g_page_directory;

void initialize_paging();
void allow_paging();
void load_directory_table(page_directory* directory);
uint32_t page_to_address(uint32_t page_number);
uint32_t address_to_page(uint32_t address);
void page_map(page_directory* directory, uint32_t vadd, uint32_t padd, int flags);
void page_unmap(uint32_t vadd);
void update_pages_array(uint32_t page_num, int is_on);
uint32_t page_alloc();
uint32_t rand_page_alloc(uint32_t num_of_pages);
uint32_t check_bits_in_byte(uint8_t byte, int num_of_bits);
void page_free(uint32_t page_num);
void initialize_page_table_entry(page_table_entry* table_entry,
uint32_t address,
uint8_t present,    
uint8_t rw,         
uint8_t us,        
uint8_t pwt,        
uint8_t pcd,       
uint8_t accessed,
uint8_t dirty,
uint8_t pat,      
uint8_t global,     
uint8_t avl);

void initialize_paging()
{
    int i = 0;

    memset(g_pages_array, 0, PAGES_COUNT);

    
    //mappign pages for the page talbes and page directory
    for (i = 0; i< PAGE_TABLE_COUNT; i++)
    {
        update_pages_array(PAGE_DIRECTORY_START + i, 1);
    }

    //converting the page directory into address and initializing the page directory array
    g_page_directory = (page_directory*)page_to_address(PAGE_DIRECTORY_START);

    //setting all the page directory entries as not present
    for(i = 0; i < PAGE_TABLE_COUNT; i++)
    {
        initialize_page_table_entry(&g_page_directory->directory_entries[i], 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    }
    // load page directory
    load_directory_table(g_page_directory);

    //mapping kernel code and stack and heap as kernel
    for( i = 0; i < KERNEL_SOURCE_SIZE + KERNEL_STACK_SIZE + KERNEL_HEAP_SIZE; i++)
    {
        page_map(g_page_directory, KERNEL_START_ADDR + i * PAGE_SIZE, KERNEL_START_ADDR + i * PAGE_SIZE, PAGE_FLAG_KERNEL | PAGE_FLAG_READWRITE);
    }

	//mapping process stack
    for( i = 0; i < PROCESS_STACK_SIZE; i++)
    {
        page_map(g_page_directory, PROCESS_STACK + i * PAGE_SIZE, PROCESS_STACK + i * PAGE_SIZE, PAGE_FLAG_KERNEL | PAGE_FLAG_READWRITE);
    }

	for( i = 0; i < NET_RX_SIZE; i++)
    {
        page_map(g_page_directory, NET_RX + i * PAGE_SIZE, NET_RX + i * PAGE_SIZE, PAGE_FLAG_KERNEL | PAGE_FLAG_READWRITE);
    }

	//mapping user mode
    for(i = 0; i < USER_SOURCE_SIZE + USER_STACK_SIZE + USER_HEAP_SIZE; i++)
    {
        page_map(g_page_directory, USER_MODE_START + i * PAGE_SIZE, USER_MODE_START + i * PAGE_SIZE, PAGE_FLAG_USER | PAGE_FLAG_READWRITE);
    }
    
    //mapping the video memory into the physical address
    page_map(g_page_directory, VIDEO_MEM_START, VIDEO_MEM_PHYSICAL_ADDR, PAGE_FLAG_READWRITE | PAGE_FLAG_KERNEL);
    
    //mapping the page tables and page directory
    for(i= 0; i < PAGE_TABLE_COUNT; i++)
    {
        page_map(g_page_directory, page_to_address(i), page_to_address(i), PAGE_FLAG_READWRITE | PAGE_FLAG_USER);
    }

    allow_paging();
}

/*
    This function maps page into physical address
    directory: pointer to the paging directory
    vadd: virtual address
    padd: physical address 
    flags: specify the flags that should be set
*/
void page_map(page_directory* directory, uint32_t vadd, uint32_t padd, int flags)
{
    int32_t page_num= 0;
    int32_t page_table_num = 0;
    page_table* pt;
    
    //getting page and page table number
    page_table_num = vadd >> 22;
	page_num = (vadd >> 12) & 0x3ff;

    //translating the page table number into addres 
    pt = (page_table*)page_to_address(PAGE_TABLES_START + page_table_num);
    
    //initializing pde according to the flags
    initialize_page_table_entry(&directory->directory_entries[page_table_num],
    (uint32_t)pt >> 12,
    1,
    (flags & PAGE_FLAG_READWRITE) ? 1 : 0, 
    (flags & PAGE_FLAG_USER) ? 0 : 1,
    0,
    0,
    0,
    0,
    0,
    0,
    0);

    //initializing the pte according to the flags
    initialize_page_table_entry(&pt->table_entries[page_num],
    padd >> 12,
    1,
    (flags & PAGE_FLAG_READWRITE) ? 1 : 0, 
    (flags & PAGE_FLAG_USER) ? 0 : 1,
    0,
    0,
    0,
    0,
    0,
    0,
    0);

    //updatin the page array
    update_pages_array(address_to_page(padd), 1);
    
}

/*
    This function unmaps page from physical address
    vadd: virtual address
*/
void page_unmap(uint32_t vadd)
{
    uint32_t page_num= 0;
    uint32_t page_table_num = 0;
    page_table* pt;
    
    //getting page and page table number
    page_table_num = vadd >> 22;
	page_num = (vadd >> 12) & 0x3ff;

    //translating the page table number into addres 
    pt = (page_table*)page_to_address(PAGE_TABLES_START + page_table_num);
    initialize_page_table_entry(&pt->table_entries[page_num],
    0,
    0,
    0, 
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0);

}

/*
    This function translates physical address to page number
    address: page physical address
*/
uint32_t address_to_page(uint32_t address)
{
    return (address - PAGES_START_ADDR) / PAGE_SIZE;
}

/*
    This function translates page number to physical address
    page_number: page number
*/
uint32_t page_to_address(uint32_t page_number)
{
    return page_number * PAGE_SIZE + PAGES_START_ADDR;
}

/*
    This function updates the pages array due to page alloc or free
    page_num: the number of the page into the g_pages_array
    is_on: 0 if alloc 0 if free
*/
void update_pages_array(uint32_t page_num, int is_on)
{
    //getting the byte index into the g_pages_array to be updated
    uint32_t page_byte_array = page_num / 8;
    //getting the bit in the page array byte index that will be updated
    uint32_t page_bit = page_byte_array % 8;

    //updating according to is_on
    g_pages_array[page_byte_array] = is_on == 1 ? g_pages_array[page_byte_array] | (1 << page_bit) : g_pages_array[page_byte_array] & ~(1 <<page_bit);
    
}

/*
    This function allocates page 
    retrun:page num
*/
uint32_t page_alloc()
{
    uint8_t curr_bit = 0;

    for(int i = 0; i < PAGES_COUNT; i++)
    {
        //if there is a space for page to be mapped
        if(g_pages_array[i] != 0xFF)
        {
            //saving the curr pages bit array
            curr_bit= g_pages_array[i];

            //going through the bits in the bit array
            for(int j = 0; j < BITS_IN_BYTE; j++)
            {
                //moving to the next bit
                curr_bit =(1 << j);
                
                // if an empty bit
                if(!(g_pages_array[i] & curr_bit))
                {
                    //  initializing the page with NULL
                    memset((uint32_t*)(page_to_address(curr_bit * BITS_IN_BYTE + j)), NULL, PAGE_SIZE);
                    //updating the page_array 
                    update_pages_array(curr_bit * BITS_IN_BYTE + j, 1);

                    //returning the page num    
                    return curr_bit * BITS_IN_BYTE +j;
                }

            }
        }
    }

    // no pages left and allocation failed returning null
    return NULL;
}
/*
    This function allocates randomely a certain number of pages
    @param num_of_pages: pages to be mapped under or equal to 8
    returns page num of the first page allocated
*/
uint32_t rand_page_alloc(uint32_t num_of_pages)
{

    uint8_t curr_bit = 0;
    uint32_t shift_num = 0;
    int i = 0;
    int j = 0;


    while(true)
    {
        uint32_t rand_byte = rand() % PAGES_COUNT;
        //if there is a space for pages to be mapped
        if(g_pages_array[rand_byte] != 0xFF)
        {
            //saving the curr pages bit array
            shift_num = check_bits_in_byte(g_pages_array[rand_byte], num_of_pages);

            if(shift_num != -1)
            {
                
                //going through the bits in the bit array
                for(j = 0; j < num_of_pages; j++)
                {
                    //moving to the next bit
                    curr_bit = 1 << (shift_num + j);
                
                    // if an empty bit
                    if(!(g_pages_array[rand_byte] & curr_bit))
                    {
                        //  initializing the page with NULL
                        memset((uint32_t*)(page_to_address(curr_bit * BITS_IN_BYTE + j)), NULL, PAGE_SIZE);
                        //updating the page_array 
                        update_pages_array(curr_bit * BITS_IN_BYTE + j, 1);
                    }

                }

                //returning the page num    
                return (1 << shift_num) * BITS_IN_BYTE +j;
            }
        }
    }
}

/*
    This function checks if there is enoghth frre bits in byte fot page alloc
    @param byte: the checked byte
    @param num_of_bits: the num_o_bits to be checked 
*/
uint32_t check_bits_in_byte(uint8_t byte, int num_of_bits)
{
    int i = 0;
    uint8_t curr_byte = 0;
    uint8_t checked_byte = 0;
    int is_found = 1;

    // saving the curr byte
    for (int j = 0; j < 8; j++)
    {
        curr_byte = 1 << j;

        // if not enough empty bits
        if (8 - num_of_bits < j)
        {
            return -1;
        }

        // going through the curr byte
        checked_byte = curr_byte;

        while (i < num_of_bits)
        {
            // if the checked byte has no space
            if (byte & checked_byte)
            {
                is_found = 0;
                break;
            }

            // going to the next bit
            i++;
            checked_byte = 1 << (j + i);
        }
        // if the curr checked byte is good
        if (is_found == 1)
        {
            return j;
        }

        i = 0;
        is_found = 1;
    }

    return -1;
}

void page_free(uint32_t page_num)
{
    update_pages_array(page_num, 0);
}

/*
    This function loads the g_page_directory tale into the cr3 register
    @param directory: the page directory pointer 
*/
void load_directory_table(page_directory* directory)
{
    asm volatile("mov 0, %eax;");
    asm volatile("mov %%eax, %%cr3" : : "a" (directory));
}

/*
    This function initializes the pte
    table_entry: pointer to the pte
    address: page physical address
    present: is present
    rw: read write or read only
    us: user or kernel
    pwt:    nulled
    pcd:    nulled
    accessed:   nulled
    dirty:  nulled
    pat:    nulled
    global: nulled
    avl:    nulled
*/
void initialize_page_table_entry(page_table_entry* table_entry,
uint32_t address,
uint8_t present,    
uint8_t rw,         
uint8_t us,        
uint8_t pwt,        
uint8_t pcd,       
uint8_t accessed,
uint8_t dirty,
uint8_t pat,      
uint8_t global,     
uint8_t avl)
{
    table_entry->address = address;
    table_entry->present = present;
    table_entry->rw = rw;
    table_entry->us = us;
    table_entry->pwt = pwt;
    table_entry->pcd = pcd;
    table_entry->accessed = accessed;
    table_entry->dirty = dirty;
    table_entry->pat = pat;
    table_entry->global = global;
    table_entry->avl = avl;
}

/*
    This function changes the cr0 register value in order to allow paging
*/
void allow_paging()
{
	asm("movl %cr0, %eax");
    //enabling paging
	asm("orl $0x80000000, %eax");
	asm("movl %eax, %cr0");
}