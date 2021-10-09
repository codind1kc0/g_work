#include  "memory.h"

static  fmem_list    gfmem_list = {0};
static  t_list       gvmem_list = {0};

static void fmem_init(byte*  mem, uint size)
{
    uint max = 0;
    uint i   = 0;
    fmem_node*  p =  NULL;

    max = size / (FM_ALLOC_SIZE + FM_NODE_SIZE);
    gfmem_list.max   = max;
    gfmem_list.nbase = (fmem_node* )mem;
    gfmem_list.ubase = (fmem_unit* )((uint)mem + max * FM_NODE_SIZE);
    gfmem_list.node  = (fmem_node* )mem;

    p = gfmem_list.node;
    for (i=0; i < max-1; i++) {
        fmem_node* current = (fmem_node* )AddressOff(p, i);
        fmem_node* next    = (fmem_node* )AddressOff(p, i+1);

        current -> next = next;
    }
  ((fmem_node* )AddressOff(p, i)) -> next = NULL;
}

static void*  fmem_alloc(void)
{
    void* ret = NULL;
    if (NULL != gfmem_list.node) {
        fmem_node* alloc = gfmem_list.node;
        gfmem_list.node  = alloc -> next;

        int index = AddressIndex(alloc, gfmem_list.nbase);
        ret = AddressOff(gfmem_list.ubase, index);

        //remenber alloc address.
        alloc -> ptr = ret;
    }       

    return ret;
}

static int  fmem_free(void* ptr)
{
    int ret = 0;
 
    if ( ptr ) { 
        uint index = AddressIndex((fmem_unit*)ptr, gfmem_list.ubase);
        fmem_node* node = AddressOff(gfmem_list.nbase, index);

        if ( (index < gfmem_list.max) && (node -> ptr == ptr) ) {
            node -> next = gfmem_list.node;
            gfmem_list.node = node;
            // free ok.
            ret = 1; 
        } else {
            ret ^= ret;
        }
    }
    
    return ret;
}

static void vmem_init(byte* mem, uint size)
{
    if (mem) {
	    list_init(&gvmem_list);

	    vmem_head* head = (vmem_head* )mem;
		head -> used = 0;
		head -> free = size -VM_HEAD_SIZE;
		head -> ptr  = (byte* )AddressOff(head, 1); 
	
	    list_add_tail(&gvmem_list, (list_node *)head);
	}
}

static void* vmem_alloc(uint size)
{
	list_node* pos = NULL;
	vmem_head* ret = NULL;
	
    list_for_each(&gvmem_list, pos) {
		vmem_head* current  = (vmem_head* )pos; 
		if( (current -> free) >= (size + VM_HEAD_SIZE)) {
            /* 计算新结点起始位置！ */
			ret = (vmem_head* )((uint)(current -> ptr) + (uint)(current -> used) \
			                   + ((uint)(current -> free)) - (size + VM_HEAD_SIZE));
			ret -> used = size;
			ret -> free = 0;
			ret -> ptr  = (byte* )AddressOff(ret, 1);

			current -> free -= (size + VM_HEAD_SIZE);

			list_add_after((list_node* )current, (list_node* )ret);
			break;
		}
	}

	return (void *)(ret ? ret->ptr : NULL);
}

static int  vmem_free(void* ptr)
{
    int ret = 0;
    
    if (NULL != ptr) {
	    list_node* pos = NULL;

		list_for_each(&gvmem_list, pos) {
	        vmem_head* head = (vmem_head* )pos;

			if (IsEqual( head -> ptr, ptr) ) {
            	vmem_head* prev_node = (vmem_head* )(head -> head.prev);
        
	   		    prev_node -> free += ( (head -> used) + (head -> free) + VM_HEAD_SIZE);
			    del_node ((list_node* )head);
        
        		ret = 1;
				break;
			}
		}
	 }

	return ret;
}

void   memory_mod_init(byte* mem, uint size)
{
    byte* fmem  = mem;
	uint  fsize = size/2;
	
	byte* vmem  = AddressOff(fmem, fsize);
	uint  vsize = size - fsize;

    fmem_init(fmem, fsize);
	vmem_init(vmem, vsize);
}

void*  m_malloc(uint  size)
{
    void* ret = NULL;

    if (size <= FM_ALLOC_SIZE) {
	    /* in case, fmem can't used! */
	    if ( !(ret = fmem_alloc()) ) {
		    ret = vmem_alloc(size);
		}    
	} else {
	    ret = vmem_alloc(size);
	}

	return ret;
}

int m_free(void* ptr)
{
    int ret = 0;
    
	if ( ptr ) {
	    if ( !(ret = fmem_free(ptr)) ) {
	        ret = vmem_free(ptr);
	    } 
    }

	return ret;
}



