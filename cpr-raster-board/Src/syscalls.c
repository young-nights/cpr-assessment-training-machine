/**
 * @file    syscalls.c
 * @brief   Newlib retarget stubs for bare-metal builds
 *
 * Minimal syscall implementations required by newlib (libc_nano).
 * They override the always-failing stubs from libnosys:
 *   - _write  -> USART1 debug console (UART1_Putc)
 *   - _read   -> no input source (returns EOF)
 *   - _close  -> no filesystem (always fails)
 *   - _lseek  -> not supported
 *
 * Change Logs:
 * Date           Author      Notes
 * 2026-11-13     coder       the first version
 */
#include "raster_sys.h"

/**
 * @brief  Write bytes to a file descriptor (stdout/stderr -> USART1)
 * @param  file  file descriptor, ignored (both fd 1 and 2 map to USART1)
 * @param  ptr   data buffer
 * @param  len   number of bytes to write
 * @return number of bytes written
 */
int _write(int file, char *ptr, int len)
{
    (void)file;

    for (int i = 0; i < len; i++) {
        UART1_Putc(ptr[i]);
    }
    return len;
}

/**
 * @brief  Read bytes from a file descriptor (no input source)
 * @param  file  file descriptor, ignored
 * @param  ptr   receive buffer, unused
 * @param  len   maximum bytes to read, unused
 * @return 0 (EOF)
 */
int _read(int file, char *ptr, int len)
{
    (void)file;
    (void)ptr;
    (void)len;
    return 0;
}

/**
 * @brief  Close a file descriptor (no filesystem on bare metal)
 * @param  file  file descriptor, ignored
 * @return -1 (failure)
 */
int _close(int file)
{
    (void)file;
    return -1;
}

/**
 * @brief  Set file position (not supported)
 * @param  file  file descriptor, ignored
 * @param  ptr   offset, unused
 * @param  dir   seek origin, unused
 * @return 0
 */
int _lseek(int file, int ptr, int dir)
{
    (void)file;
    (void)ptr;
    (void)dir;
    return 0;
}
