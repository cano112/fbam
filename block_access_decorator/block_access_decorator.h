#ifndef FBAM_BLOCK_ACCESS_DECORATOR_H
#define FBAM_BLOCK_ACCESS_DECORATOR_H

#define _GNU_SOURCE

#include "../common/commons.h"

#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <dlfcn.h>
#include <sys/param.h>
#include <stdio.h>
#include <stdlib.h>

typedef int (*pread_function_type)(int fd, void *buf, size_t count, off_t offset);

ssize_t pread(int fd, void *buf, size_t count, off_t offset);

ssize_t pread64(int fd, void *buf, size_t count, off_t offset);

ssize_t read(int fd, void *buf, size_t count);

#endif //FBAM_BLOCK_ACCESS_DECORATOR_H
