#ifndef FBAM_BLOCK_ACCESS_DECORATOR_H
#define FBAM_BLOCK_ACCESS_DECORATOR_H

#define _GNU_SOURCE

#include "constants.h"

#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <dlfcn.h>
#include <sys/param.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <fcntl.h>
#include <stdarg.h>
#include <errno.h>
#include <fnmatch.h>

ssize_t pread(int fd, void *buf, size_t count, off_t offset);

typedef int (*pread_function_type)(int fd, void *buf, size_t count, off_t offset);

ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset);

typedef int (*pwrite_function_type)(int fd, const void *buf, size_t count, off_t offset);

ssize_t read(int fd, void *buf, size_t count);

typedef int (*read_function_type)(int fd, void *buf, size_t count);

ssize_t write(int fd, const void *buf, size_t count);

typedef int (*write_function_type)(int fd, const void *buf, size_t count);

typedef int (*open_function_type)(const char *path, int oflags, ...);

int open(const char *path, int oflags, ...);

typedef int (*close_function_type)(int fildes);

int close(int fildes);

typedef int (*execve_function_type)(const char* filename, char* const argv[], char* const envp[]);

int execve(const char* filename, char* const argv[], char* const envp[]);

typedef int (*execveat_function_type)(int dirfd, const char *pathname, char *const argv[], char *const envp[], int flags);

int execveat(int dirfd, const char *pathname, char *const argv[], char *const envp[], int flags);

typedef int (*fexecve_function_type)(int fd, char *const argv[], char *const envp[]);

int fexecve(int fd, char *const argv[], char *const envp[]);

typedef int (*execle_function_type)(const char *path, const char *arg, ...);

int execle(const char *path, const char *arg, ...);

typedef int (*execvpe_function_type)(const char *file, char *const argv[], char *const envp[]);

int execvpe(const char *file, char *const argv[], char *const envp[]);

#endif //FBAM_BLOCK_ACCESS_DECORATOR_H
