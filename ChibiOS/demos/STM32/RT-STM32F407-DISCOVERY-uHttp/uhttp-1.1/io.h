
#if !defined(__IO_H__)
#define __IO_H__

#include <stddef.h>

extern int _close(int fd);
extern int _open(const char *name, int mode);
extern int _read(int fd, void *buffer, size_t count);
extern int _write(int fd, const void *buffer, size_t count);
extern int _seek(int fd, long offset, int origin);
extern long _tell(int fd);

extern int _ioctl(int fd, int cmd, void *buffer);
extern long _filelength(int fd);

#endif /* __IO_H__ */
/*** EOF ***/