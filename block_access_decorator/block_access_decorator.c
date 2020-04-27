#include "block_access_decorator.h"


void report_and_exit(const char* msg) {
    perror(msg);
    exit(-1); /* EXIT_FAILURE */
}

void append_log(FILE* log_file, char* function_name, char* file_path, size_t count, off_t offset)
{
    char log_message[MAX_LOG_LEN];
    snprintf(log_message, MAX_LOG_LEN, "{\"fuction_name\": \"%s\", \"file_path\": \"%s\", \"size\": \"%zu\", \"offset\": \"%ld\"}\n",
            function_name, file_path, count, offset);
    fputs(log_message, log_file);
    fflush(log_file);
}

void log_file_access(char* function_name, int fd, size_t count, off_t offset)
{
    FILE *access_log_file = fopen(LOG_FILE_PATH, "a+");
    if (access_log_file != NULL)
    {
        char fd_path[MAXPATHLEN];
        char file_path[MAXPATHLEN];
        snprintf(fd_path, MAXPATHLEN, "/proc/self/fd/%d", fd);
        readlink(fd_path, file_path, MAXPATHLEN);
        append_log(access_log_file, function_name, file_path, count, offset);
    }
}

ssize_t pread(int fd, void *buf, size_t count, off_t offset)
{
    log_file_access("pread", fd, count, offset);
    pread_function_type original_pread = (pread_function_type)dlsym(RTLD_NEXT, "pread");
    return original_pread(fd, buf, count, offset);
}

ssize_t read(int fd, void *buf, size_t count)
{
    log_file_access("read", fd, count, 0);
    pread_function_type original_pread = (pread_function_type)dlsym(RTLD_NEXT, "read");
    return original_pread(fd, buf, count, 0);
}



