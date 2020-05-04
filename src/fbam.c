#include "fbam.h"

char* log_file_path;
char* work_dir;
char* command;
char* ld_preload_env;

FILE* access_log_file;
int logs_count = 0;


static int initialize() {
    work_dir = getenv(HF_VAR_FS_MONIT_PATH_FILTER);
    command = getenv(HF_VAR_FS_MONIT_COMMAND);
    log_file_path = getenv(HF_VAR_FS_MONIT_LOGFILE);
    ld_preload_env = getenv(LD_PRELOAD);
    access_log_file = fopen(log_file_path, "a+");
    return 0;
}
__attribute__((section(".init_array"))) static void *ctr = &initialize;

void report_and_exit(const char* msg) {
    perror(msg);
    exit(-1);
}

char* format_timestamp(char *buffer, struct timeval time)
{
    struct tm* tm_info;
    tm_info = localtime(&(time.tv_sec));
    char tmpbuf[TIMESTAMP_BUFFER_LEN];
    strftime(tmpbuf, TIMESTAMP_BUFFER_LEN/2, "%Y-%m-%dT%H:%M:%S", tm_info);
    snprintf(buffer, TIMESTAMP_BUFFER_LEN, "%s.%06ld", tmpbuf, time.tv_usec);
    return buffer;
}

void _fflush(FILE* log_file)
{
    logs_count = 0;
    fflush(log_file);
}

void flush_log(char* log_message, FILE* log_file)
{
    fputs(log_message, log_file);
    logs_count++;
    if (logs_count == BUFFER_SIZE)
    {
        _fflush(log_file);
    }
}

void append_block_log(FILE* log_file, struct timeval time, char* function_name, char* file_path, size_t count, size_t real_count, off_t offset)
{
    char log_message[MAX_LOG_LEN];
    char time_buffer[TIMESTAMP_BUFFER_LEN];
    snprintf(log_message, MAX_LOG_LEN,
            "{\"time\": \"%s\", "
             "\"command\": \"%s\", "
             "\"parameter\": \"%s\", "
             "\"value\": {"
                 "\"file_path\": \"%s\", "
                 "\"size\": \"%zu\", "
                 "\"real_size\": \"%zu\", "
                 "\"offset\": \"%ld\"}}\n",
             format_timestamp(time_buffer, time), command, function_name, file_path, count, real_count, offset);
    flush_log(log_message, log_file);
}

void append_open_log(FILE* log_file, struct timeval time, char* function_name, char* file_path, int oflags)
{
    char log_message[MAX_LOG_LEN];
    char time_buffer[TIMESTAMP_BUFFER_LEN];
    snprintf(log_message, MAX_LOG_LEN,
            "{\"time\": \"%s\", "
             "\"command\": \"%s\", "
             "\"parameter\": \"%s\", "
             "\"value\": {"
                 "\"file_path\": \"%s\", "
                 "\"oflags\": \"%d\"}}\n",
             format_timestamp(time_buffer, time), command, function_name, file_path, oflags);
    flush_log(log_message, log_file);
}

void append_close_log(FILE* log_file, struct timeval time, char* function_name, char* file_path)
{
    char log_message[MAX_LOG_LEN];
    char time_buffer[TIMESTAMP_BUFFER_LEN];
    snprintf(log_message, MAX_LOG_LEN,
            "{\"time\": \"%s\", "
             "\"command\": \"%s\", "
             "\"parameter\": \"%s\", "
             "\"value\": {"
                 "\"file_path\": \"%s\"}}, ",
             format_timestamp(time_buffer, time), command, function_name, file_path);
    flush_log(log_message, log_file);
}

struct timeval get_timestamp() {
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return tv;
}

void log_file_block_access(char* function_name, int fd, size_t count, size_t real_count, off_t offset, struct timeval timestamp)
{
    if (access_log_file != NULL)
    {
        char fd_path[MAXPATHLEN];
        char file_path[MAXPATHLEN];
        snprintf(fd_path, MAXPATHLEN, "/proc/self/fd/%d", fd);
        ssize_t path_len = readlink(fd_path, file_path, MAXPATHLEN);
        file_path[path_len] = '\0';
        if (strstr(file_path, work_dir)) 
        {
            append_block_log(access_log_file, timestamp, function_name, file_path, count, real_count, offset);
        }
    }
}

void log_file_open(char* function_name, int fd, int oflags, struct timeval timestamp)
{
    if (access_log_file != NULL)
    {
        char fd_path[MAXPATHLEN];
        char file_path[MAXPATHLEN];
        snprintf(fd_path, MAXPATHLEN, "/proc/self/fd/%d", fd);
        ssize_t path_len = readlink(fd_path, file_path, MAXPATHLEN);
        file_path[path_len] = '\0';
        if (strstr(file_path, work_dir)) 
        {
            append_open_log(access_log_file, timestamp, function_name, file_path, oflags);
        }
    }
}

void log_file_close(char* function_name, int fd, struct timeval timestamp)
{
    if (access_log_file != NULL)
    {
        char fd_path[MAXPATHLEN];
        char file_path[MAXPATHLEN];
        snprintf(fd_path, MAXPATHLEN, "/proc/self/fd/%d", fd);
        ssize_t path_len = readlink(fd_path, file_path, MAXPATHLEN);
        file_path[path_len] = '\0';
        if (strstr(file_path, work_dir)) 
        {
            append_close_log(access_log_file, timestamp, function_name, file_path);
        }
    }
}

ssize_t pread(int fd, void *buf, size_t count, off_t offset)
{
    struct timeval timestamp = get_timestamp();
    pread_function_type original_pread = (pread_function_type)dlsym(RTLD_NEXT, FUN_PREAD);
    ssize_t read = original_pread(fd, buf, count, offset);
    log_file_block_access(FUN_PREAD, fd, count, read, offset, timestamp);
    return read;
}

ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset)
{
    struct timeval timestamp = get_timestamp();
    pwrite_function_type original_pwrite = (pwrite_function_type)dlsym(RTLD_NEXT, FUN_PWRITE);
    ssize_t written = original_pwrite(fd, buf, count, offset);
    log_file_block_access(FUN_PWRITE, fd, count, written, offset, timestamp);
    return written;
}

ssize_t read(int fd, void *buf, size_t count)
{
    struct timeval timestamp = get_timestamp();
    off_t offset = lseek(fd, 0, SEEK_CUR);
    read_function_type original_read = (read_function_type)dlsym(RTLD_NEXT, FUN_READ);
    ssize_t read = original_read(fd, buf, count);
    log_file_block_access(FUN_READ, fd, count, read, offset, timestamp);
    return read;
    
}

ssize_t write(int fd, const void *buf, size_t count)
{
    struct timeval timestamp = get_timestamp();
    off_t offset = lseek(fd, 0, SEEK_CUR);
    write_function_type original_write = (write_function_type)dlsym(RTLD_NEXT, FUN_WRITE);
    ssize_t written = original_write(fd, buf, count);
    log_file_block_access(FUN_WRITE, fd, count, written, offset, timestamp);
    return written;
}
int open(const char *path, int oflags, ...)
{
    struct timeval timestamp = get_timestamp();
    open_function_type original_open = (open_function_type)dlsym(RTLD_NEXT, FUN_OPEN);
    int fd = original_open(path, oflags);
    log_file_open(FUN_OPEN, fd, oflags, timestamp);
    return fd;
}

int close(int fildes)
{
    struct timeval timestamp = get_timestamp();
    close_function_type original_close = (close_function_type)dlsym(RTLD_NEXT, FUN_CLOSE);
    log_file_close(FUN_CLOSE, fildes, timestamp);
    return original_close(fildes);
}

void load_env_variable_string(char *buffer, char* name, char* value)
{
    snprintf(buffer, MAXPATHLEN, "%s=%s", name, value);
}

void build_new_exec_env(char *new_envp[], char *const envp[])
{
    int n = 0;
    while(envp[n] != NULL)
    {
        char *current = envp[n];
        char *buffer = malloc((strlen(current) + 1) * sizeof(char));
        strcpy(buffer, current);
        new_envp[n] = buffer;
        n++;
    }
    char *ld_buffer = malloc(MAXPATHLEN * sizeof(char));
    load_env_variable_string(ld_buffer, LD_PRELOAD, ld_preload_env);
    
    char *filter_buffer = malloc(MAXPATHLEN * sizeof(char));
    load_env_variable_string(filter_buffer, HF_VAR_FS_MONIT_PATH_FILTER, work_dir);
    
    char *command_buffer = malloc(MAXPATHLEN * sizeof(char));
    load_env_variable_string(command_buffer, HF_VAR_FS_MONIT_COMMAND, command);
    
    char *file_path_buffer = malloc(MAXPATHLEN * sizeof(char));
    load_env_variable_string(file_path_buffer, HF_VAR_FS_MONIT_LOGFILE, log_file_path);
    
    new_envp[n] = ld_buffer;
    new_envp[n+1] = filter_buffer;
    new_envp[n+2] = command_buffer;
    new_envp[n+3] = file_path_buffer;
    new_envp[n+4] = NULL;
}

int execve(const char* filename, char* const argv[], char* const envp[])
{
    char *new_envp[ARG_MAX];
    build_new_exec_env(new_envp, envp);
    _fflush(access_log_file); // force flush
    
    execve_function_type original_execve = (execve_function_type)dlsym(RTLD_NEXT, FUN_EXECVE);  
    int ret = original_execve(filename, argv, new_envp);
    initialize(); // initialize global variables again
}

int execveat(int dirfd, const char *pathname, char *const argv[], char *const envp[], int flags)
{
    char *new_envp[ARG_MAX];
    build_new_exec_env(new_envp, envp);
    _fflush(access_log_file); // force flush
    
    execveat_function_type original_execveat = (execveat_function_type)dlsym(RTLD_NEXT, FUN_EXECVEAT);  
    int ret = original_execveat(dirfd, pathname, argv, new_envp, flags);
    initialize(); // initialize global variables again
}





