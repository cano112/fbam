#include "block_access_decorator.h"


void report_and_exit(const char* msg) {
    perror(msg);
    exit(-1); /* EXIT_FAILURE */
}

void log_file_access(char* function_name, int fd, size_t count, off_t offset)
{
    key_t key = ftok(QUEUE_PATH_NAME, PROJECT_ID);
    if (key < 0) report_and_exit("couldn't get key...");
    printf("Got key\n");

    int queue_id = msgget(key, 0666 | IPC_CREAT);
    if (queue_id < 0) report_and_exit("couldn't get queue id...");
    printf("Got queue id\n");

    char fd_path[MAXPATHLEN];
    char file_path[MAXPATHLEN];
    snprintf(fd_path, MAXPATHLEN, "/proc/self/fd/%d", fd);
    readlink(fd_path, file_path, MAXPATHLEN);

    file_access_message message;
    message.type = MESSAGE_TYPE;
    message.function_name = function_name;
    message.count = count;
    message.offset = offset;
    message.file_path = file_path;
    msgsnd(queue_id, &message, sizeof(message), 0);
    printf("Message sent\n");
}

ssize_t pread(int fd, void *buf, size_t count, off_t offset)
{
    log_file_access("pread", fd, count, offset);
    pread_function_type original_pread = (pread_function_type)dlsym(RTLD_NEXT, "pread");
    return original_pread(fd, buf, count, offset);
}

ssize_t pread64(int fd, void *buf, size_t count, off_t offset)
{
    log_file_access("pread64", fd, count, offset);
    pread_function_type original_pread = (pread_function_type)dlsym(RTLD_NEXT, "pread64");
    return original_pread(fd, buf, count, offset);
}

ssize_t read(int fd, void *buf, size_t count)
{
    log_file_access("read", fd, count, 0);
    pread_function_type original_pread = (pread_function_type)dlsym(RTLD_NEXT, "read");
    return original_pread(fd, buf, count, 0);
}



