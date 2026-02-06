#ifndef PD_UNISTD_H
#define PD_UNISTD_H

#include <sys/types.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int close(int fd);
ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, const void *buf, size_t count);

#ifdef __cplusplus
}
#endif

#endif
