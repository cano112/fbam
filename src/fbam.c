#include "fbam.h"

char* log_file_path;
char* work_dir;
char* command;

FILE* access_log_file;
int logs_count = 0;


static int initialize() {
    work_dir = getenv(HF_VAR_FS_MONIT_PATH_FILTER);
    command = getenv(HF_VAR_FS_MONIT_COMMAND);
    log_file_path = getenv(HF_VAR_FS_MONIT_LOGFILE);
    access_log_file = fopen(log_file_path, "a+");
    return 0;
}
__attribute__((section(".init_array"))) static void *ctr = &initialize;

void report_and_exit(const char* msg) {
    perror(msg);
    exit(-1);
}

char* format_timestamp(char *buffer, time_t time)
{
    struct tm* tm_info;
    tm_info = localtime(&time);
    strftime(buffer, TIMESTAMP_BUFFER_LEN, "%Y-%m-%d-%H-%M-%S", tm_info);
    return buffer;
}

void append_log(FILE* log_file, time_t time, char* function_name, char* file_path, size_t count, off_t offset)
{
    char log_message[MAX_LOG_LEN];
    char time_buffer[TIMESTAMP_BUFFER_LEN];
    snprintf(log_message, MAX_LOG_LEN,
            "{\"command\": \"%s\", \"timestamp\": \"%s\", \"function_name\": \"%s\", \"file_path\": \"%s\", \"size\": \"%zu\", \"offset\": \"%ld\"}\n",
             command, format_timestamp(time_buffer, time), function_name, file_path, count, offset);
    fputs(log_message, log_file);
    logs_count++;
    if (logs_count == BUFFER_SIZE)
    {
        logs_count = 0;
        fflush(log_file);
    }
}

void log_file_access(char* function_name, int fd, size_t count, off_t offset)
{
    time_t timestamp = time(NULL);
    if (access_log_file != NULL)
    {
        char fd_path[MAXPATHLEN];
        char file_path[MAXPATHLEN];
        snprintf(fd_path, MAXPATHLEN, "/proc/self/fd/%d", fd);
        readlink(fd_path, file_path, MAXPATHLEN);
        if (strstr(file_path, work_dir)) 
        {
            append_log(access_log_file, timestamp, function_name, file_path, count, offset);
        }
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
    pread_function_type original_read = (pread_function_type)dlsym(RTLD_NEXT, "read");
    return original_read(fd, buf, count, 0);
}




