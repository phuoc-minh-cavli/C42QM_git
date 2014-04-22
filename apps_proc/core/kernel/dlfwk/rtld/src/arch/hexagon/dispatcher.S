/* Dispatcher for sys_* functions */

/* DO NOT MODIFY THIS FILE.  ITS ORDER IS SIGNIFICANT */

#include "dispatch_table.h"

    .text
    .global _exit
    .type _exit, @function
    .global sys_access
    .type sys_access, @function
    .global sys_clock
    .type sys_clock, @function
    .global sys_close
    .type sys_close, @function
    .global sys_closedir
    .type sys_closedir, @function
    .global sys_errno
    .type sys_errno, @function
    .global sys_exit
    .type sys_exit, @function
    .global sys_fcntl
    .type sys_fcntl, @function
    .global sys_flen
    .type sys_flen, @function
    .global sys_fstat
    .type sys_fstat, @function
    .global sys_ftell
    .type sys_ftell, @function
    .global sys_getcwd
    .type sys_getcwd, @function
    .global sys_getpid
    .type sys_getpid, @function
    .global sys_gettimeofday
    .type sys_gettimeofday, @function
    .global sys_get_cmdline
    .type sys_get_cmdline, @function
    .global sys_heapinfo
    .type sys_heapinfo, @function
    .global sys_isatty
    .type sys_isatty, @function
    .global sys_iserror
    .type sys_iserror, @function
    .global sys_mkdir
    .type sys_mkdir, @function
    .global sys_Mtxdst
    .type sys_Mtxdst, @function
    .global sys_Mtxinit
    .type sys_Mtxinit, @function
    .global sys_Mtxlock
    .type sys_Mtxlock, @function
    .global sys_Mtxunlock
    .type sys_Mtxunlock, @function
    .global sys_open
    .type sys_open, @function
    .global sys_opendir
    .type sys_opendir, @function
    .global sys_read
    .type sys_read, @function
    .global sys_readc
    .type sys_readc, @function
    .global sys_readdir
    .type sys_readdir, @function
    .global sys_remove
    .type sys_remove, @function
    .global sys_rename
    .type sys_rename, @function
    .global sys_rmdir
    .type sys_rmdir, @function
    .global sys_sbrk
    .type sys_sbrk, @function
    .global sys_seek
    .type sys_seek, @function
    .global sys_stat
    .type sys_stat, @function
    .global sys_statvfs
    .type sys_statvfs, @function
    .global sys_system
    .type sys_system, @function
    .global sys_time
    .type sys_time, @function
    .global sys_Tlsalloc
    .type sys_Tlsalloc, @function
    .global sys_Tlsfree
    .type sys_Tlsfree, @function
    .global sys_Tlsget
    .type sys_Tlsget, @function
    .global sys_Tlsset
    .type sys_Tlsset, @function
    .global sys_tmpnam
    .type sys_tmpnam, @function
    .global sys_write
    .type sys_write, @function
    .global sys_write0
    .type sys_write0, @function
    .global sys_writec
    .type sys_writec, @function
    .global sys_writecreg
    .type sys_writecreg, @function
    .global sys_fork
    .type sys_fork, @function
    .global sys_execve
    .type sys_execve, @function

    .data
    .extern __dispatch_table

    .text

Dispatch(_exit)
Dispatch(sys_access)
Dispatch(sys_clock)
Dispatch(sys_close)
Dispatch(sys_closedir)
Dispatch(sys_errno)
Dispatch(sys_exit)
Dispatch(sys_fcntl)
Dispatch(sys_flen)
Dispatch(sys_fstat)
Dispatch(sys_ftell)
Dispatch(sys_getcwd)
Dispatch(sys_getpid)
Dispatch(sys_gettimeofday)
Dispatch(sys_get_cmdline)
Dispatch(sys_heapinfo)
Dispatch(sys_isatty)
Dispatch(sys_iserror)
Dispatch(sys_mkdir)
Dispatch(sys_Mtxdst)
Dispatch(sys_Mtxinit)
Dispatch(sys_Mtxlock)
Dispatch(sys_Mtxunlock)
Dispatch(sys_open)
Dispatch(sys_opendir)
Dispatch(sys_read)
Dispatch(sys_readc)
Dispatch(sys_readdir)
Dispatch(sys_remove)
Dispatch(sys_rename)
Dispatch(sys_rmdir)
Dispatch(sys_sbrk)
Dispatch(sys_seek)
Dispatch(sys_stat)
Dispatch(sys_statvfs)
Dispatch(sys_system)
Dispatch(sys_time)
Dispatch(sys_Tlsalloc)
Dispatch(sys_Tlsfree)
Dispatch(sys_Tlsget)
Dispatch(sys_Tlsset)
Dispatch(sys_tmpnam)
Dispatch(sys_write)
Dispatch(sys_write0)
Dispatch(sys_writec)
Dispatch(sys_writecreg)
Dispatch(sys_fork)
Dispatch(sys_execve)

