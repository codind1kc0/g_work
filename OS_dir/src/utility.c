
#include "utility.h"

//0x103c6
void delay(int n)
{
    while( n > 0 )
    {
        int i = 0;
        int j = 0;
        
        for(i=0; i<1000; i++)
        {
            for(j=0; j<700; j++)
            {
                asm volatile ("nop\n\t");
            }
        }
        
        n--;
    }
}

char* str_cpy(char* dest_str, const char*  src_str, uint copy_num)
{
    char* ret = dest_str;
	uint i = 0;

	for (i = 0; *(src_str + i)&&(i < copy_num); i++) {
        *(dest_str + i)	 = *(src_str + i);
 	}   
	
	*(dest_str + i) = 0;
	return (ret); 
}

int str_len(const char*  str)
{
    int ret = 0;

    if (str) {
	    for ( ret = 0; str[ret]!='\0'; ++ret )
		{
		}
	}

	return ret;
}

int str_cmp(const char*  dest, const char*  src, uint n)
{
    int l_len = str_len(dest);
    int r_len = str_len(src);
	int min   = ( l_len < r_len ? l_len :r_len );
    int ret   = IsEqual(l_len, r_len);
    
    if ( !IsEqual(dest, src) ) {
	/* differ must compare. */
		int i = 0;
	    min = min<n ? min:n ;

		for (i=0; (i<min)&&ret; ++i) {
		    if ( !(ret = IsEqual(dest[i], src[i])) ) {
			    break;
			}  
		}
	}

	return ret;
}

