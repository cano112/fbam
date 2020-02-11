#ifndef FBAM_COMMONS_H
#define FBAM_COMMONS_H

#include <unistd.h>

#define LOG_FILE_PATH "/tmp/file_access.log"
#define MAX_LOG_LEN (MAXPATHLEN + 50)
#define PROJECT_ID 123
#define QUEUE_PATH_NAME "/dev/null"

long MESSAGE_TYPE = 1;

typedef struct file_access_message {
    long type;
    char* function_name;
    char* file_path;
    size_t count;
    off_t offset;
} file_access_message;

#endif //FBAM_COMMONS_H
