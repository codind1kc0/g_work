#ifndef UTILITY_H
#define UTILITY_H

#include "type.h"

#define  AddressOff(a, i)    ((void *)((uint)(a) + (i)*sizeof(*a)))
#define  AddressIndex(b, a)  ( (uint)( ((uint)(b) - (uint)(a))/sizeof(*b)) )

#define  IsEqual(a, b)                               ({ (uint)(a) == (uint)(b) ? 1 : 0 ;})

#define  OffsetOf(type, member)                      ( (uint) &(((type*)0)->member) )                        
#define  Dim(array)                                  ((uint)(sizeof(array)/sizeof(*array)))

#define  Container(ptr, type, member)                                                  \
({                                    const typeof(((type*)0)->member)* __mptr = (ptr);\
									 (type* )((uint)__mptr - OffsetOf(type, member));  \
})

#define read_port(port_num)  ({                         \
                               register char ret_val = 0;\
							   asm volatile(             \
							   "in %%dx, %%al\n\t"       \
							   :"=a"(ret_val)            \
							   :"d"(port_num)            \
							   :);                       \
                               ret_val;                  \
							 })

#define write_port(port_num, cmd) ({                            \
							          asm volatile(             \
							          "out %%al, %%dx\n\t"      \
							          ::"a"(cmd),"d"(port_num)  \
							          :);                       \

void delay(int n);
char* str_cpy(char* dest_str, const char* src_str, uint copy_num);

int str_len(const char*  str);
int str_cmp(const char* dest, const char* src, uint n);

#endif


