#include "process.h"
#include "kheap.h"

process_t* process_create(void)
{
    process_t *process =
        (process_t*)kmalloc(sizeof(process_t));

    if (!process)
        return 0;

    process->page_directory = vmm_create_page_directory();

    process->code_start = 0;
    process->code_size = 0;
    process->stack_top = 0;

    return process;
}

void process_destroy(process_t *process)
{
    if (!process)
        return;

    vmm_destroy_page_directory(process->page_directory);

    kfree(process);
}