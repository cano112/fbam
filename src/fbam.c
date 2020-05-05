#include "fbam.h"

char* log_file_path;
char* work_dir_pattern;
char* command;
char* ld_preload_env;

FILE* access_log_file;
int logs_count = 0;

static char* format_timestamp(char *buffer, struct timeval time) {
    struct tm* tm_info;
    tm_info = localtime(&(time.tv_sec));
    char tmpbuf[TIMESTAMP_BUFFER_LEN];
    strftime(tmpbuf, TIMESTAMP_BUFFER_LEN/2, "%Y-%m-%dT%H:%M:%S", tm_info);
    snprintf(buffer, TIMESTAMP_BUFFER_LEN, "%s.%06ld", tmpbuf, time.tv_usec);
    return buffer;
}

static struct timeval get_timestamp() {
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return tv;
}

static void flush(FILE* log_file) {
    logs_count = 0;
    fflush(log_file);
}

static void flush_log(char* log_message, FILE* log_file) {
    fputs(log_message, log_file);
    logs_count++;
    if (logs_count == BUFFER_SIZE) {
        flush(log_file);
    }
}

static void free_string_array(char** in) {
    int save_errno = errno;
    for (char** it = in; it && *it; ++it) {
        free(*it);
    }
    free(in);
    errno = save_errno;
}

static char** build_string_array(const char *arg, va_list *args)
{
    char **result = 0;
    size_t size = 0;
    for (const char* it = arg; it; it = va_arg(*args, const char *)) {
        result = realloc(result, (size + 2) * sizeof(const char *));
        if (!result) {
            return NULL;
        }
        char* copy = strdup(it);
        if (!copy) {
            goto undo;
        }
        result[size++] = copy;
        result[size] = 0;
    }
    return result;

undo:
    /* Return an empty array.  */
    free_string_array(result);
    return NULL;
}

void log_file_block_access(char* function_name, int fd, size_t count, size_t real_count, off_t offset, struct timeval timestamp) {
    if (access_log_file != NULL) {
        char fd_path[MAXPATHLEN];
        char file_path[MAXPATHLEN];
        snprintf(fd_path, MAXPATHLEN, "/proc/self/fd/%d", fd);
        ssize_t path_len = readlink(fd_path, file_path, MAXPATHLEN);
        file_path[path_len] = '\0';
        if (fnmatch(work_dir_pattern, file_path, 0)) {
            char log_message[MAX_LOG_LEN];
            char time_buffer[TIMESTAMP_BUFFER_LEN];
            char* log_message_template = "{\"time\": \"%s\", "
                                         "\"command\": \"%s\", "
                                         "\"parameter\": \"%s\", "
                                         "\"value\": {"
                                             "\"file_path\": \"%s\", "
                                             "\"size\": \"%zu\", "
                                             "\"real_size\": \"%zu\", "
                                             "\"offset\": \"%ld\"}}\n";
            snprintf(log_message, MAX_LOG_LEN, log_message_template,
                     format_timestamp(time_buffer, timestamp), command, function_name, file_path, count, real_count, offset);
            flush_log(log_message, access_log_file);
        }
    }
}

void log_file_open(char* function_name, int fd, int oflags, struct timeval timestamp) {
    if (access_log_file != NULL) {
        char fd_path[MAXPATHLEN];
        char file_path[MAXPATHLEN];
        snprintf(fd_path, MAXPATHLEN, "/proc/self/fd/%d", fd);
        ssize_t path_len = readlink(fd_path, file_path, MAXPATHLEN);
        file_path[path_len] = '\0';
        if (fnmatch(work_dir_pattern, file_path, 0)) {
            char log_message[MAX_LOG_LEN];
            char time_buffer[TIMESTAMP_BUFFER_LEN];
            char* log_message_template = "{\"time\": \"%s\", "
                                         "\"command\": \"%s\", "
                                         "\"parameter\": \"%s\", "
                                         "\"value\": {"
                                             "\"file_path\": \"%s\", "
                                             "\"oflags\": \"%d\"}}\n";
            snprintf(log_message, MAX_LOG_LEN, log_message_template,
                     format_timestamp(time_buffer, timestamp), command, function_name, file_path, oflags);
            flush_log(log_message, access_log_file);
        }
    }
}

void log_file_close(char* function_name, int fd, struct timeval timestamp) {
    if (access_log_file != NULL) {
        char fd_path[MAXPATHLEN];
        char file_path[MAXPATHLEN];
        snprintf(fd_path, MAXPATHLEN, "/proc/self/fd/%d", fd);
        ssize_t path_len = readlink(fd_path, file_path, MAXPATHLEN);
        file_path[path_len] = '\0';
        if (fnmatch(work_dir_pattern, file_path, 0)) {
            char log_message[MAX_LOG_LEN];
            char time_buffer[TIMESTAMP_BUFFER_LEN];
            char* log_message_template = "{\"time\": \"%s\", "
                                         "\"command\": \"%s\", "
                                         "\"parameter\": \"%s\", "
                                         "\"value\": {"
                                             "\"file_path\": \"%s\"}}\n";
            snprintf(log_message, MAX_LOG_LEN, log_message_template,
                     format_timestamp(time_buffer, timestamp), command, function_name, file_path);
            flush_log(log_message, access_log_file);
        }
    }
}

static int initialize() {
    work_dir_pattern = getenv(HF_VAR_FS_MONIT_PATH_PATTERN);
    command = getenv(HF_VAR_FS_MONIT_COMMAND);
    log_file_path = getenv(HF_VAR_FS_MONIT_LOGFILE);
    ld_preload_env = getenv(LD_PRELOAD);
    access_log_file = fopen(log_file_path, "a+");
    return 0;
}
__attribute__((section(".init_array"))) static void *ctr = &initialize;

ssize_t pread(int fd, void *buf, size_t count, off_t offset) {
    struct timeval timestamp = get_timestamp();
    pread_function_type original_pread = (pread_function_type)dlsym(RTLD_NEXT, FUN_PREAD);
    ssize_t read = original_pread(fd, buf, count, offset);
    log_file_block_access(FUN_PREAD, fd, count, read, offset, timestamp);
    return read;
}

ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset) {
    struct timeval timestamp = get_timestamp();
    pwrite_function_type original_pwrite = (pwrite_function_type)dlsym(RTLD_NEXT, FUN_PWRITE);
    ssize_t written = original_pwrite(fd, buf, count, offset);
    log_file_block_access(FUN_PWRITE, fd, count, written, offset, timestamp);
    return written;
}

ssize_t read(int fd, void *buf, size_t count) {
    struct timeval timestamp = get_timestamp();
    off_t offset = lseek(fd, 0, SEEK_CUR);
    read_function_type original_read = (read_function_type)dlsym(RTLD_NEXT, FUN_READ);
    ssize_t read = original_read(fd, buf, count);
    log_file_block_access(FUN_READ, fd, count, read, offset, timestamp);
    return read;
}

ssize_t write(int fd, const void *buf, size_t count) {
    struct timeval timestamp = get_timestamp();
    off_t offset = lseek(fd, 0, SEEK_CUR);
    write_function_type original_write = (write_function_type)dlsym(RTLD_NEXT, FUN_WRITE);
    ssize_t written = original_write(fd, buf, count);
    log_file_block_access(FUN_WRITE, fd, count, written, offset, timestamp);
    return written;
}

int open(const char *path, int oflags, ...) {
    struct timeval timestamp = get_timestamp();
    open_function_type original_open = (open_function_type)dlsym(RTLD_NEXT, FUN_OPEN);
    int fd;
    if (oflags & O_CREAT)
    {
        va_list args;
        va_start(args, oflags);
        int mode = va_arg(args, int);
        va_end(args);
        fd = original_open(path, oflags, mode);
    } else 
    {
        fd = original_open(path, oflags);
    }
    
    log_file_open(FUN_OPEN, fd, oflags, timestamp);
    return fd;
}

int close(int fildes) {
    struct timeval timestamp = get_timestamp();
    close_function_type original_close = (close_function_type)dlsym(RTLD_NEXT, FUN_CLOSE);
    log_file_close(FUN_CLOSE, fildes, timestamp);
    return original_close(fildes);
}

void load_env_variable_string(char *buffer, char* name, char* value) {
    snprintf(buffer, MAXPATHLEN, "%s=%s", name, value);
}

void build_new_exec_env(char *new_envp[], char *const envp[]) {
    int n = 0;
    while (envp[n] != NULL) {
        char *current = envp[n];
        char *buffer = malloc((strlen(current) + 1) * sizeof(char));
        strcpy(buffer, current);
        new_envp[n] = buffer;
        n++;
    }
    
    // TODO: Do not add variable if it's already present
    char *ld_buffer = malloc(MAXPATHLEN * sizeof(char));
    load_env_variable_string(ld_buffer, LD_PRELOAD, ld_preload_env);
    
    char *pattern_buffer = malloc(MAXPATHLEN * sizeof(char));
    load_env_variable_string(pattern_buffer, HF_VAR_FS_MONIT_PATH_PATTERN, work_dir_pattern);
    
    char *command_buffer = malloc(MAXPATHLEN * sizeof(char));
    load_env_variable_string(command_buffer, HF_VAR_FS_MONIT_COMMAND, command);
    
    char *file_path_buffer = malloc(MAXPATHLEN * sizeof(char));
    load_env_variable_string(file_path_buffer, HF_VAR_FS_MONIT_LOGFILE, log_file_path);
    
    new_envp[n] = ld_buffer;
    new_envp[n+1] = pattern_buffer;
    new_envp[n+2] = command_buffer;
    new_envp[n+3] = file_path_buffer;
    new_envp[n+4] = NULL;
}

int execve(const char* filename, char* const argv[], char* const envp[]) {
    char *new_envp[ARG_MAX];
    build_new_exec_env(new_envp, envp);
    flush(access_log_file);

    execve_function_type original_execve = (execve_function_type)dlsym(RTLD_NEXT, FUN_EXECVE);  
    int ret = original_execve(filename, argv, new_envp);
    free_string_array(new_envp);
    initialize();
    flush(access_log_file);
}

int execveat(int dirfd, const char *pathname, char *const argv[], char *const envp[], int flags) {
    char *new_envp[ARG_MAX];
    build_new_exec_env(new_envp, envp);
    flush(access_log_file);

    execveat_function_type original_execveat = (execveat_function_type)dlsym(RTLD_NEXT, FUN_EXECVEAT);  
    int ret = original_execveat(dirfd, pathname, argv, new_envp, flags);
    free_string_array(new_envp);
    initialize();
    flush(access_log_file);
}

int fexecve(int fd, char *const argv[], char *const envp[]) {
    char *new_envp[ARG_MAX];
    build_new_exec_env(new_envp, envp);
    flush(access_log_file);

    fexecve_function_type original_fexecve = (fexecve_function_type)dlsym(RTLD_NEXT, FUN_FEXECVE);  
    int ret = original_fexecve(fd, argv, new_envp);
    free_string_array(new_envp);
    initialize();
    flush(access_log_file);
}

int execle(const char *path, const char *arg, ...) {
    va_list args;
    va_start(args, arg);
    char **argv = build_string_array(arg, &args);
    char *const *envp = va_arg(args, char *const *);
    va_end(args);

    char *new_envp[ARG_MAX];
    build_new_exec_env(new_envp, envp);
    flush(access_log_file);

    execle_function_type original_execle = (execle_function_type)dlsym(RTLD_NEXT, FUN_EXECLE);  
    int ret = original_execle(path, arg, new_envp);
    free_string_array(new_envp);
    free_string_array(argv);
    initialize();
    flush(access_log_file);
}

int execvpe(const char *file, char *const argv[], char *const envp[]) {
    char *new_envp[ARG_MAX];
    build_new_exec_env(new_envp, envp);
    flush(access_log_file);

    execvpe_function_type original_execvpe = (execvpe_function_type)dlsym(RTLD_NEXT, FUN_EXECVPE);  
    int ret = original_execvpe(file, argv, new_envp);
    free_string_array(new_envp);
    initialize();
    flush(access_log_file);
}
