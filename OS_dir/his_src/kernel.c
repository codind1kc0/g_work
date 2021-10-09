#include "kernel.h"

int set_descriptor_value(descriptor* pdesc, uint base, uint limit, uint attr)
{
    int ret = 0;

	if (ret = (NULL != pdesc)) {
        pdesc -> limit1 = (limit & 0xffff);
     	pdesc -> base_addr_h1 = (base & 0xffff);
    	pdesc -> base_addr_h2 = (base & (0xff0000)) >> 16;
    	pdesc -> attribute1 = (attr & 0xff);
    	pdesc -> attribute2_limit2 = ((attr >> 8) & 0xf0) | ((limit >> 16) & 0x0f );
    	pdesc -> base_addr_h3 = (base & 0xff000000) >> 24;
    }

	return ret;
}

int get_descriptor_value(descriptor* pdesc, uint* pbase, uint* plimit, uint* pattr)
{
    int ret = 0;

	if (ret = (pdesc && pbase && plimit && pattr)) {
        *pbase = (pdesc -> base_addr_h1) | (pdesc -> base_addr_h2 << 16) | (pdesc -> base_addr_h3 << 24);
		*plimit = (pdesc -> limit1) | ((pdesc -> attribute2_limit2 & 0xf) << 16);
		*pattr =  (pdesc -> attribute1) | ((pdesc -> attribute2_limit2 & 0xf0) << 8); // not 4, meaning have 0000
	}

	return ret;

}
