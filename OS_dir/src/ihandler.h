
#ifndef  IHANDLER_H
#define  IHANDLER_H


#define  decl_handler(name)    void  name##_entry(void);\
                               void  name(void)

decl_handler(page_fault);
decl_handler(segment_fault);
decl_handler(timer);
decl_handler(keyboard);
decl_handler(sys_call);

#endif

