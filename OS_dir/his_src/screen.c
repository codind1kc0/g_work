#include "screen.h"

static byte g_posw = 0;
static byte g_posh = 0;
static Print_color g_color = SCREEN_RED;

void set_print_pos(byte w, byte h)
{
    uint temp_result = 0;

    if (w >= POSW_MAX){ 
	// illegal deal...
	    w = 0;
	} 
	
	if (h >= POSH_MAX) {
	    h = 0;
	}

	g_posw = w;
	g_posh = h;

    temp_result = (h*80 + w);
	asm volatile (
	"movl %0, %%ebx\n\t"
	"movw $0x03d4, %%dx\n\t"
	"movb $0x0e, %%al\n\t"
	"outb %%al, %%dx\n\t"
	"movw $0x03d5, %%dx\n\t"
	"movb %%bh, %%al\n\t"
	"outb %%al, %%dx\n\t"
	
	"movw $0x03d4, %%dx\n\t"
	"movb $0x0f, %%al\n\t"
	"outb %%al, %%dx\n\t"
	"movw $0x03d5, %%dx\n\t"
	"movb %%bl, %%al\n\t"
	"outb %%al, %%dx\n\t"
	:
    :"r"(temp_result)
	:"eax", "ebx", "edx"
	);

}

void set_print_color(Print_color c)
{
    g_color = c;   
}


/*
 * print a charctter in the screen.
 */
byte print_char(char c)
{
	byte  ph = 0, pw = 0, ret = 0;
	uint  pos_chg = 0;

    if ('\n' == c || '\r' == c) {
	    set_print_pos (0, g_posh + 1);

	} else {
	    pw = g_posw;
	    ph = g_posh;
	
	    if ( (POSW_MAX > pw) && (POSH_MAX > ph)) {
			pos_chg = (pw + ph * 80 ) << 1;
    	    asm volatile (
	    	//	"pusha\n\t"
   			 	"movl %1, %%edi\n\t"
    			"movb %0, %%al\n\t"
    			"movb g_color, %%ah\n\t"
    			"movw %%ax, %%gs:(%%edi)\n\t" 
    		//	"popa\n\t"
    			:
    			:"r"(c), "r"(pos_chg)
    			:"%eax", "%edi"
    		);
		//set next pos (0, MAX-1)
		if (POSW_MAX <= ++pw) {
		   pw = 0;
		   ph++;
		}
		set_print_pos(pw, ph);
        }

    }	

	return ret;
}

int print_string(const char* s)
{
    int print_len = 0;
    if (NULL == s) {
	   return -1;
	} else {
	   while (*s++) {
	      if (0 ==  print_char(*(s-1))) {
              print_len++;
		  }
	   }
	}

	return print_len;
}

int print_int_hex(uint n)
{
    int pos = 0;
    char chg_save[8] = {0};
	
    for (pos = 7; n!=0; pos--) {
	    chg_save[pos] = n%16;
		n /= 16;
	}
// prnit hex
    print_string("0x");
	for (pos = 0; pos < 8; pos++) {
	    if (chg_save[pos] < 10) {
		    print_char(chg_save[pos]+'0');
		} else {
		    print_char(chg_save[pos]+'a'-10);
		}
	}

	return 0;
}

int print_int_dec(int n)
{
    int ret = 0;

	if (n < 0) {
	    print_char('-');
		n = -n;
	}

    if (n < 10) {
	    print_char('0' + n);
		return ret;
	} else if (n >= 10) {
	    print_int_dec(n/10);
		print_int_dec(n%10);
		return ret;
	}
}


void clear_screen(void)
{
    int w = 0, h = 0;
    set_print_pos(w, h);

	for (h = 0;h < (POSH_MAX); h++) {
        for (w = 0; w < (POSW_MAX); w++) {
	         //print_string("                    ");
			 print_char(' ');
	    }
		     //set_print_pos(0, h);
	}
    
	set_print_pos(0, 0);
}
