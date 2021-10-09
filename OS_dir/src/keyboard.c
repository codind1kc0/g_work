
#include "keyboard.h"
#include "task.h"

static volatile  key_code_node  gKCBuffer = {0};

static volatile  t_queue    gWaitKeyTask = {0}; 

typedef struct  key_code_{
    byte ascii1;
    byte ascii2;
    byte scode;
    byte kcode;
}key_code;

static const key_code gKeyMap [] = {
    /* 0x00 - none */ { 0,   0,  0x00, 0x00},
    /* 0x01 -  esc */ { 0,   0,  0x01, 0x1b},
    /* 0x02 -  1/! */ {'1', '!', 0x02, 0x31},
    /* 0x03 -  2/@ */ {'2', '@', 0x03, 0x32},
	/* 0x04 -  3/# */ {'3', '#', 0x04, 0x33},
    /* 0x05 -  4/$ */ {'4', '$', 0x05, 0x34},
    /* 0x06 -  5/% */ {'5', '%', 0x06, 0x35},
    /* 0x07 -  6/^ */ {'6', '^', 0x07, 0x36},
    /* 0x08 -  7/& */ {'7', '&', 0x08, 0x37},
    /* 0x09 -  8/  */ {'8', '*', 0x09, 0x38},
    /* 0x0a -  9/( */ {'9', '(', 0x0a, 0x39},
    /* 0x0b -  0/) */ {'0', ')', 0x0b, 0x30},
    /* 0x0c -  -/_ */ {'-', '_', 0x0c, 0x2d},
    /* 0x0d -  =/+ */ {'=', '+', 0x0d, 0x3d},
    /* 0x0e -  back*/ { 0 ,  0 , 0x0e, 0x08},
    /* 0x0f -  tab */ {'\t',0, 0x0f, 0x09},
    
    /* 0x10 -  q/Q */ {'q', 'Q', 0x10, 0x71},
    /* 0x11 -  w/W */ {'w', 'W', 0x11, 0x77},
    /* 0x12 -  e/E */ {'e', 'E', 0x12, 0x65},
    /* 0x13 -  r/R */ {'r', 'R', 0x13, 0x72},
    /* 0x14 -  t/T */ {'t', 'T', 0x14, 0x74},
    /* 0x15 -  y/Y */ {'y', 'Y', 0x15, 0x79},
    /* 0x16 -  u/U */ {'u', 'U', 0x16, 0x75},
    /* 0x17 -  i/I */ {'i', 'I', 0x17, 0x69},
    /* 0x18 -  o/O */ {'o', 'O', 0x18, 0x6f},
    /* 0x19 -  p/P */ {'p', 'P', 0x19, 0x70},
    /* 0x1a -  [/{ */ {'[', '{', 0x1a, 0x5b},
    /* 0x1b -  ]/} */ {']', '}', 0x1b, 0x5d},
    /* 0x1c -   \r */ { 0,   0,  0x1c, 0x0d},
    /* 0x1d -  ctrl*/ { 0,   0,  0x1d, 0x00},
    /* 0x1e -  a/A */ {'a', 'A', 0x1e, 0x61},
    /* 0x1f -  s/S */ {'s', 'S', 0x1f, 0x73},
    /* 0x20 -  d/D */ {'d', 'Q', 0x20, 0x64},
    /* 0x21 -  f/F */ {'f', 'W', 0x21, 0x66},
    /* 0x22 -  g/G */ {'g', 'E', 0x22, 0x67},
    /* 0x23 -  h/H */ {'h', 'R', 0x23, 0x68},
    /* 0x24 -  j/J */ {'j', 'T', 0x24, 0x6a},
    /* 0x25 -  k/K */ {'k', 'Y', 0x25, 0x6b},
    /* 0x26 -  l/L */ {'l', 'U', 0x26, 0x6c},
    /* 0x27 -  ;/: */ {';', 'I', 0x27, 0x3b},
    /* 0x28 -  '/" */ {'\'','\"',0x28, 0x27},
    /* 0x29 - '`/~'*/ {'`' ,'~', 0x29, 0xee}, /* temp  */
    /* 0x2a - lshift*/{ 0,   0,  0x2a, 0x00},
	/* 0x2b -  \/| */ {'\\','|' ,0x2b, 0xdc},
    /* 0x2c -  z/Z */ {'z', 'Z', 0x2c, 0x7a},
    /* 0x2d -  x/X */ {'x', 'X', 0x2d, 0x78},
    /* 0x2e -  c/C */ {'c', 'C', 0x2e, 0x63},
    /* 0x2f -  v/V */ {'v', 'V', 0x2f, 0x76},
    /* 0x30 -  b/B */ {'b', 'B', 0x30, 0x62},
    /* 0x31 -  n/N */ {'n', 'N', 0x31, 0x6e},
    /* 0x32 -  m/M */ {'m', 'M', 0x32, 0x6d},
    /* 0x33 -  ,/< */ {',', '<', 0x33, 0x2c},
    /* 0x34 -  ./> */ {'.', '>', 0x34, 0x2e},
	/* 0x35 -  //? */ {'/', '?', 0x35, 0x2f},
    /* 0x36 - rshift*/{ 0,   0,  0x36, 0x00},
    /* 0x37 -  *    */{'*',  0,  0x37, 0x6a},
    /* 0x38 -  none*/ { 0,   0,  0x00, 0x00},
    /* 0x39 -  ' ' */ {' ',  0,  0x39, 0x20},
    
    /* 0x3a -  none*/ { 0,   0,  0x3a, 0x00},
    /* 0x3b -   F1 */ { 0,   0,  0x3b, 0x00},
    /* 0x3c -   F2 */ { 0,   0,  0x3c, 0x00},
    /* 0x3d -   F3 */ { 0,   0,  0x3d, 0x00},
    /* 0x3e -   F4 */ { 0,   0,  0x3e, 0x00},
    /* 0x3f -   F5 */ { 0,   0,  0x3f, 0x00},
    /* 0x40 -   F6 */ { 0,   0,  0x40, 0x00},
    /* 0x41 -   F7 */ { 0,   0,  0x41, 0x00},
    /* 0x42 -   F8 */ { 0,   0,  0x42, 0x00},
    /* 0x43 -   F9 */ { 0,   0,  0x43, 0x00},
    /* 0x44 -   F10*/ { 0,   0,  0x44, 0x00},
    /* 0x45 -numlock*/{ 0,   0,  0x45, 0x00},
    /* 0x46 -  none*/ { 0,   0,  0x00, 0x00},
    
    /* 0x47 -  7/home*/ {'7',  0, 0x47, 0x37},
    /* 0x48 -  8/Up  */ {'8',  0, 0x48, 0x38},
    /* 0x49 -  9/Pg_U*/ {'9',  0, 0x49, 0x39},
    /* 0x4a -  '-'   */ {'-',  0, 0x4a, 0x2d},
    /* 0x4b -  4/Left*/ {'4',  0, 0x4b, 0x34},
    /* 0x4c -  5     */ {'5',  0, 0x4c, 0x35},
    /* 0x4d -  6/righT*/{'6',  0, 0x4d, 0x36},
    /* 0x4e -  +     */ {'+',  0, 0x4e, 0x2b},
    /* 0x4f -  1/Pg_U*/ {'1',  0, 0x4f, 0x31},
    /* 0x50 -  2/Pg_D*/ {'2',  0, 0x50, 0x32},
    /* 0x51 -  3     */ {'3',  0, 0x51, 0x33},
    /* 0x52 -  0/ins */ {'0',  0, 0x52, 0x30},
    /* 0x53 -  del   */ { 0 ,  0, 0x53, 0x00},

	/* 0x54 - Enter    */ {  0,  0, 0x54, 0x0d},
	/* 0x55 - ???      */ {  0 , 0, 0x00, 0x00},
	/* 0x56 - ???      */ {  0 , 0, 0x00, 0x00},
	/* 0x57 - F11      */ {  0 , 0, 0x57, 0x7a},
	/* 0x58 - F12      */ {  0 , 0, 0x58, 0x7b},
	/* 0x59 - ???      */ {  0 , 0, 0x00, 0x00},
	/* 0x5a - ???      */ {  0 , 0, 0x00, 0x00},
	/* 0x5b - Left Win */ {  0 , 0, 0x5b, 0x5b},
	/* 0x5c - Right Win*/ {  0 , 0, 0x5c, 0x5c},
	/* 0x5d - Apps     */ {  0 , 0, 0x5d, 0x5d},
	/* 0x5e - Pause    */ {  0 , 0, 0x5e, 0x13}
};


enum {
    /* H : pressed  | scan_code  | virtual_code  | ascii */
    KEY_RELEASE = 0x00000000,
	KEY_PRESS   = 0x01000000,
};

uint fetch_key_code(void)
{
    uint ret = 0;

    if ( gKCBuffer.count > 0 ) {
	    ret = gKCBuffer.buff[gKCBuffer.head];
		gKCBuffer.head =  ( gKCBuffer.head + 1 ) %  gKCBuffer.max;
		--gKCBuffer.count;
	}

	return ret;
}


static void  store_key_code(uint kc)
{
    if ( gKCBuffer.count < gKCBuffer.max ) {
	    gKCBuffer.buff[gKCBuffer.tail] = kc;
		gKCBuffer.tail = ( gKCBuffer.tail + 1 ) %  gKCBuffer.max;
		++gKCBuffer.count;
	} else {
	    fetch_key_code();
		store_key_code(kc);
	}
}


static uint key_type(byte sc)
{
    uint ret = KEY_PRESS;
    
	ret = ( sc & 0x80 ?  KEY_RELEASE : ret );

	return ret; 
}

static uint pause_handler(byte sc)
{
    uint ret = 0;
	static uint  c_pause =  0;
	static byte  c_pause_code[] = {0xe1, 0x1d, 0x45, 0xe1, 0x9d, 0xc5};
    
	ret = ( 0xe1 == sc || c_pause );
	if ( *(byte*)AddressOff(c_pause_code, c_pause) == sc) {
	    ++c_pause;
	} else {
	    c_pause = 0;
		ret = 0;
	}

	if ( c_pause == Dim(c_pause_code) ) {
        c_pause = 0;
        put_scan_code(0x5e);
        put_scan_code(0xde);
	}
    
	return ret;
}

static uint IsShift(byte sc)
{
    return (0x2a == sc) || (0x36 == sc);
}

static uint IsCapsLock(byte sc)
{
    return (0x3a == sc);
}

static uint IsNumLock(byte sc)
{
    return (0x45 == sc);
}

static uint IsNumPadKey(byte sc, uint E0)
{
    uint ret = 0;

    static const  byte  cNumScanCode[] = {
	    0x52, 0x53, 0x4f, 0x50, 0x51, 0x48, 0x4c, 0x40,
		0x47, 0x48, 0x49, 0x35, 0x37, 0x4a, 0x4e, 0x1c
	};
    static const  byte  cNumE0[Dim(cNumScanCode)] = {
	    0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 1, 0, 0, 0, 1
	}; 

	int i = 0;
	for ( i=0; i<Dim(cNumScanCode); i++ ) {
	    byte* pc = AddressOff(cNumScanCode, i);
	    byte* pe = AddressOff(cNumE0, i);

		if (*pc == sc && *pe == E0) {
		    ret = 1;
			break;
		}
	}
    
	return ret;
}

static uint make_num_code(key_code* nkc, uint shift, uint num) {
    uint ret_code = 0;

	key_code* pkc = NULL;
	static const key_code  cNumKeyMap[] = {
            /* 0x52 -  0/ins */ {'0',  0, 0x52, 0x30},
            /* 0x4f -  1/Pg_U*/ {'1',  0, 0x4f, 0x31},
            /* 0x50 -  2/Pg_D*/ {'2',  0, 0x50, 0x32},
            /* 0x51 -  3     */ {'3',  0, 0x51, 0x33},
            /* 0x4b -  4/Left*/ {'4',  0, 0x4b, 0x34},
            /* 0x4c -  5     */ {'5',  0, 0x4c, 0x35},
            /* 0x4d -  6/righT*/{'6',  0, 0x4d, 0x36},
 	        /* 0x47 -  7/home*/ {'7',  0, 0x47, 0x37},
            /* 0x48 -  8/Up  */ {'8',  0, 0x48, 0x38},
            /* 0x49 -  9/Pg_U*/ {'9',  0, 0x49, 0x39},
            /* 0x35 -  '/'   */	{'/', '/',0x35, 0x6f},
            /* 0x37 -  *     */ {'*', '*',0x37, 0x6a},
	        /* 0x4a -  '-'   */ {'-', '-',0x4a, 0x2d},
            /* 0x4e -  '+'   */ {'+', '+',0x4e, 0x2b},
            /* 0x1c -        */ { 0 ,  0, 0x1c, 0x0d},
            /* end..         */ { 0 ,  0, 0x00, 0x00},
    	};
	
	int i = 0;
	pkc = AddressOff(cNumKeyMap, i);
    for (i=0; pkc->scode; i++) {
	    if (pkc->scode == nkc->scode) {
		    pkc = nkc;
			break;
		}
		
		i++;
	    pkc = AddressOff(cNumKeyMap, i);
	}
    if ( IsEqual(pkc, nkc) ) {
		if ( !num ) {
	  	  	ret_code = (pkc->scode << 16) | (pkc->kcode << 8) | pkc->ascii2; 
		} else {
	    	if ( !shift ) {
	        	ret_code = (pkc->scode << 16) | (pkc->kcode << 8) | pkc->ascii1; 
			} else {
	        	ret_code = (pkc->scode << 16) | (pkc->kcode << 8) | pkc->ascii2; 
			}
		}
	}

	return ret_code;
}

static uint make_normal_code(key_code* mkc, uint shift, uint caps)
{
    uint ret_code = 0;
 	
	if ( !caps ) {
	    if ( !shift ) {
	        ret_code = (mkc->scode << 16) | (mkc->kcode << 8) | mkc->ascii1; 
		} else {
	        ret_code = (mkc->scode << 16) | (mkc->kcode << 8) | mkc->ascii2; 
		}
	} else {
	     if ( ('a' <= mkc->ascii1)  && ('z' >= mkc->ascii1) ) {
	        if ( !shift ) {
	       	 	ret_code = (mkc->scode << 16) | (mkc->kcode << 8) | mkc->ascii2; 	
		 	} else {
	        	ret_code = (mkc->scode << 16) | (mkc->kcode << 8) | mkc->ascii1; 	
		 	}
    	 } else { 
	     	if ( !shift ) {
	        	ret_code = (mkc->scode << 16) | (mkc->kcode << 8) | mkc->ascii1; 
		 	} else {
	        	ret_code = (mkc->scode << 16) | (mkc->kcode << 8) | mkc->ascii2; 
		 	}
		 }
	}

	return ret_code;
}

static uint make_code(key_code* pkc, uint shift, uint capslock, uint numlock, uint E0)
{
    uint ret = 0;

    if ( IsNumPadKey(pkc->scode, E0) ) {
        ret = make_num_code(pkc, shift, numlock); 
	} else {
	    ret = make_normal_code(pkc, shift, capslock); 
	}
 
	return ret;
}

static uint normal_key_handler(byte sc)
{
    uint ret = 0;
    /* deal with: E0, shift, ctrl, capslock, numlock  */
	static uint E0 = 0;
	static uint c_shift = 0;
	static uint c_capslock = 0;
	static uint c_numlock = 0;
      
	if ( 0xe0 == sc ) {
	    E0 = 1; 
        ret = 1;
	} else {
	    uint pressed = key_type(sc);
        key_code* key = NULL;

		if ( !pressed ) {
		   /* deal with release key  */
		    sc -= 0x80;
		}
        
        key = AddressOff(gKeyMap, sc);
        /* some key undef. is 0
		 * other can code.
		 * */
		if ( ret = !!key->scode ) {
		    uint code = 0;

            if ( IsShift(sc) ) {
			    c_shift = !!pressed;
			} else if ( IsCapsLock(sc) && pressed  ) {
			    c_capslock = !c_capslock;
			} else if ( IsNumLock(sc) && pressed) {
			    c_numlock = !c_numlock;
			}

			code = pressed | make_code(key, c_shift, c_capslock, c_numlock, E0);
			store_key_code(code);
			E0 = 0;

	    }

    }

	return ret;

}

void put_scan_code(byte sc)
{
    if ( pause_handler(sc) ) {
	    /* deal E1  */
	} else if ( normal_key_handler(sc)  ) {
	    /*  deal normal key/E0  */
	} else {
	    /* E2...  */
	}
}

void keyboard_mod_init(void)
{
    gKCBuffer.max = 2;
    gKCBuffer.count = 0;
    gKCBuffer.head  = 0;
    gKCBuffer.tail  = 0;

	queue_init((t_queue* )&gWaitKeyTask);
}

static void notify_all(uint key_code)
{
    event_t evt = {KeyEvent, (uint)&gWaitKeyTask, key_code, 0};
    event_schedule(NOTIFY, &evt);
}

void notify_keycode(void)
{
    uint kc = fetch_key_code();
	if ( kc ) {
	    notify_all(kc);
	}
}

void key_call_handler(uint cmd, uint param1, uint param2)
{
    if ( param1 )  {
	    uint key_code = fetch_key_code();
		if (key_code) {
			uint* ret = (uint* )param1;
			*ret = key_code;
            notify_all(key_code);

		} else {
	       // can't use this stack variable...
		   // event_t evt = {KeyEvent, (uint)&gWaitKeyTask, param1, 0};
		   // event_schedule(WAIT, &evt);
		   
		   event_t* evt = create_event(KeyEvent, (uint)&gWaitKeyTask, param1, 0);
		   event_schedule(WAIT, evt);
		}

	} else {
	/* ret address error. */
	    return;
	}
}

