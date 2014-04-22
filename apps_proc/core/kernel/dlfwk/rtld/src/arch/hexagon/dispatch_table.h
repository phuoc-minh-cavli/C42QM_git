/* Dispatch table definitions for sys_* functions */

/* DO NOT MODIFY THIS FILE.  ITS ORDER IS SIGNIFICANT */
/* Depends on disaptch_table.S in the loader */

#define _DISP_OFF__exit               0
#define _DISP_OFF_sys_access          4
#define _DISP_OFF_sys_clock           8
#define _DISP_OFF_sys_close          12
#define _DISP_OFF_sys_closedir       16
#define _DISP_OFF_sys_errno          20
#define _DISP_OFF_sys_exit           24
#define _DISP_OFF_sys_fcntl          28
#define _DISP_OFF_sys_flen           32
#define _DISP_OFF_sys_fstat          36
#define _DISP_OFF_sys_ftell          40
#define _DISP_OFF_sys_getcwd         44
#define _DISP_OFF_sys_getpid         48
#define _DISP_OFF_sys_gettimeofday   52
#define _DISP_OFF_sys_get_cmdline    56
#define _DISP_OFF_sys_heapinfo       60
#define _DISP_OFF_sys_isatty         64
#define _DISP_OFF_sys_iserror        68
#define _DISP_OFF_sys_mkdir          72
#define _DISP_OFF_sys_Mtxdst         76
#define _DISP_OFF_sys_Mtxinit        80
#define _DISP_OFF_sys_Mtxlock        84
#define _DISP_OFF_sys_Mtxunlock      88
#define _DISP_OFF_sys_open           92
#define _DISP_OFF_sys_opendir        96
#define _DISP_OFF_sys_read          100
#define _DISP_OFF_sys_readc         104
#define _DISP_OFF_sys_readdir       108
#define _DISP_OFF_sys_remove        112
#define _DISP_OFF_sys_rename        116
#define _DISP_OFF_sys_rmdir         120
#define _DISP_OFF_sys_sbrk          124
#define _DISP_OFF_sys_seek          128
#define _DISP_OFF_sys_stat          132
#define _DISP_OFF_sys_statvfs       136
#define _DISP_OFF_sys_system        140
#define _DISP_OFF_sys_time          144
#define _DISP_OFF_sys_Tlsalloc      148
#define _DISP_OFF_sys_Tlsfree       152
#define _DISP_OFF_sys_Tlsget        156
#define _DISP_OFF_sys_Tlsset        160
#define _DISP_OFF_sys_tmpnam        164
#define _DISP_OFF_sys_write         168
#define _DISP_OFF_sys_write0        172
#define _DISP_OFF_sys_writec        176
#define _DISP_OFF_sys_writecreg     180
#define _DISP_OFF_sys_fork          184
#define _DISP_OFF_sys_execve        188

#if 1
#define Dispatch(Func) \
Func: \
    {r7 = pc; \
    r6.h = #hi(Func@GOTOFF); } \
    r6.l = #lo(Func@GOTOFF); \
    r7 = sub(r7, r6); \
    r6.h = #hi(__dispatch_table@GOT); \
    r6.l = #lo(__dispatch_table@GOT); \
    r6 = add(r6, r7); \
    r6 = memw(r6); \
    r6 = memw(r6); \
    r6 = memw(r6 + #_DISP_OFF_ ## Func); \
    jumpr r6; \
    .size Func, . - Func
#else
#define Dispatch(Func) \
void Func(void) { asm("r6 = %0; jumpr r6;" : : "r"(__dispatch_table[_DISP_OFF_ ## Func]) : "r6"); }
#endif
