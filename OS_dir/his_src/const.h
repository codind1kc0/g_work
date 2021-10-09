#ifndef  CONST_H
#define  CONST_H

#define    DA_32              0x4000
#define	   DA_LIMIT_4K        0x8000
#define	   DA_DR              0x90
#define	   DA_DRW             0x92
#define    DA_DRWA            0x93
#define    DA_C               0x98
#define	   DA_CR              0x9a
#define	   DA_CCO             0x9c
#define	   DA_CCOR            0x9e

#define    DA_DPL0            0x00
#define	   DA_DPL1            0x20
#define	   DA_DPL2            0x40
#define	   DA_DPL3            0x60

#define    DA_TASKGATE        0x85
#define	   DA_386TSS          0x89
#define	   DA_386CGATE        0x8c
#define    DA_386IGATE        0x8e
#define	   DA_386TGATE        0x8f

#define    DA_LDT             0x82

#define    SA_RPL0            0
#define    SA_RPL1            1
#define    SA_RPL2            2
#define    SA_RPL3            3
    
#define    SA_TIG             0
#define	   SA_TIL             4

#define    PG_P               1    ; 页存在属性位
#define    PG_RWR             0    ; R/W 属性位值, 读/执行
#define    PG_RWW             2    ; R/W 属性位值, 读/写/执行
#define    PG_USS             0    ; U/S 属性位值, 系统级
#define    PG_USU             4    ; U/S 属性位值, 用户级

#define    GDT_CODE32_INDEX      1
#define    GDT_CFLAT32_INDEX     2 
#define    GDT_VIDEO_INDEX       3
#define    GDT_DATA32_INDEX      4
#define    GDT_DFLAT32_INDEX     5
#define    GDT_TASK_LDT_INDEX    6
#define    GDT_TASK_TSS_INDEX    7

#define    GDT_CODE32_SELECTOR       ( (GDT_CODE32_INDEX << 3) | SA_TIG | SA_RPL0 )
#define    GDT_CFLAT32_SELECTOR      ( (GDT_CFLAT32_INDEX << 3) | SA_TIG | SA_RPL0 )
#define    GDT_VIDEO_SELECTOR        ( (GDT_VIDEO_INDEX << 3) | SA_TIG | SA_RPL0 )
#define    GDT_DATA32_SELECTOR       ( (GDT_DATA32_INDEX << 3) | SA_TIG | SA_RPL0 )
#define    GDT_DFLAT32_SELECTOR      ( (GDT_DFLAT32_INDEX << 3) | SA_TIG | SA_RPL0 )
#define    GDT_TASK_LDT_SELECTOR     ( (GDT_TASK_LDT_INDEX << 3) | SA_TIG | SA_RPL0 )
#define    GDT_TASK_TSS_SELECTOR     ( (GDT_TASK_TSS_INDEX << 3) | SA_TIG | SA_RPL0 )

#define    LDT_VIDEO_INDEX      0
#define    LDT_CODE32_INDEX     1
#define    LDT_DATA32_INDEX     2

#define    LDT_VIDEO_SELECTOR      ( (LDT_VIDEO_INDEX  << 3) | SA_TIL  | SA_RPL3 )
#define    LDT_CODE32_SELECTOR     ( (LDT_CODE32_INDEX << 3) | SA_TIL  | SA_RPL3 )
#define    LDT_DATA32_SELECTOR     ( (LDT_DATA32_INDEX << 3) | SA_TIL  | SA_RPL3 )

#endif
