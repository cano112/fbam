#include "fbam.h"

FILE* access_log_file;
int logs_count = 0;
//char* jobId;

static int initialize() {
    access_log_file = fopen(LOG_FILE_PATH, "a+");

    return 0;
}
__attribute__((section(".init_array"))) static void *ctr = &initialize;

void report_and_exit(const char* msg) {
    perror(msg);
    exit(-1); /* EXIT_FAILURE */
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
            "{\"jobId\": \"%s\", \"timestamp\": \"%s\", \"function_name\": \"%s\", \"file_path\": \"%s\", \"size\": \"%zu\", \"offset\": \"%ld\"}\n",
             "TODO", format_timestamp(time_buffer, time), function_name, file_path, count, offset);
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
        append_log(access_log_file, timestamp, function_name, file_path, count, offset);
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

//int main_hook(int argc, char **argv, char **envp)
//{
//    jobId = argv[1];
//    return main_orig(argc, argv, envp);
//}
//
//int __libc_start_main(
//        int (*main)(int, char **, char **),
//        int argc,
//        char **argv,
//        int (*init)(int, char **, char **),
//        void (*fini)(void),
//        void (*rtld_fini)(void),
//        void *stack_end)
//{
//    main_orig = main;
//    typeof(&__libc_start_main) orig = dlsym(RTLD_NEXT, "__libc_start_main");
//    return orig(main_hook, argc, argv, init, fini, rtld_fini, stack_end);
//}





