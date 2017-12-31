#include <sys/types.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#include <errno.h>
#include <string.h>

#include "uart.h"

// extern char _heap_start[];
// extern char _heap_end[];

// __attribute__((weak)) caddr_t _sbrk(int incr)
// {
//     static char* cur_heap_end;
//     char* prev_heap_end;

//     if(cur_heap_end == NULL)
//     {
//         //first time init
//         cur_heap_end = _heap_start;
//         memset(_heap_start, 0, _heap_end - _heap_start);
//     }

//     prev_heap_end = cur_heap_end;

//     if((unsigned)cur_heap_end + incr > (unsigned)_heap_end)
//     {
//         errno = ENOMEM;
//         //log_error("sbrk() failed because of ENOMEM\r\n");
//         return (caddr_t)-1;
//     }

//     cur_heap_end += incr;

//     return (caddr_t) prev_heap_end;
// }

extern char _end[];
__attribute__((weak)) caddr_t _sbrk(int incr)
{
    static char* cur_heap_end;
    char* prev_heap_end;

    //first time init
    if(cur_heap_end == NULL)
        cur_heap_end = _end;

    prev_heap_end = cur_heap_end;
    cur_heap_end += incr;

    return (caddr_t) prev_heap_end;
}

__attribute__((weak)) int _write(int fd, const char* buf, int nbytes)
{
    switch(fd)
    {
        case STDOUT_FILENO:
        case STDERR_FILENO:
            uart_send(buf, nbytes);
            return nbytes;

        default:
            return -EBADF;
    }
}

__attribute__((weak)) int _close(int fd)
{
    (void)fd;
    return (0);
}

__attribute__((weak)) off_t _lseek(int fd, off_t offset, int whence)
{
    (void)fd;
    (void)offset;
    (void)whence;
    errno = ESPIPE;
    return ((off_t)-1);
}

__attribute__((weak)) int _read (int fd, char* buf, int nbytes)
{
    switch(fd)
    {
        case STDIN_FILENO:
            return uart_recv(buf, nbytes);

        default:
            return -EBADF;
    }
}

__attribute__((weak)) int _fstat(int fd, struct stat *buf)
{
    (void)fd;
    buf->st_mode = S_IFCHR; //we are always a TTY

    return (0);
}

__attribute__((weak)) int _isatty(int fd)
{
    (void)fd;
    return (1);
}
