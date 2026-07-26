#ifndef PROCESS_H
#define PROCESS_H

#include "vmm.h"

typedef struct process {

    page_directory_t *page_directory;

    unsigned int code_start;
    unsigned int code_size;

    unsigned int stack_top;

} process_t;

process_t* process_create(void);
void process_destroy(process_t *process);

#endif