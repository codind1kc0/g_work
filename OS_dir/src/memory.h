#ifndef    MEMORY_H
#define    MEMORY_H

#include "utility.h"
#include "type.h"
#include "list.h"

/* defined length */
#define  FM_ALLOC_SIZE        32
#define  FM_NODE_SIZE         sizeof(fmem_node)
#define  VM_HEAD_SIZE         sizeof(vmem_head)

typedef  byte (fmem_unit)[FM_ALLOC_SIZE];
typedef  union    _fmem_node    fmem_node;

union  _fmem_node {
    fmem_node*  next;
    fmem_unit*  ptr;
};

typedef  struct {
    fmem_node*  node;    //ptr list head node.
    fmem_node*  nbase;   //ptr manerge start addr.
    fmem_unit*  ubase;   //ptr alloc   start addr.
    uint        max;     //max node number.
}fmem_list;

/* auto length  */
typedef struct {
    list_node  head;
	uint       used;
	uint       free;
	byte*      ptr;
}vmem_head;

/* manege memory mod! */
void   memory_mod_init(byte* mem, uint size);
void*  m_malloc(uint size);
int    m_free(void* ptr);

#endif
