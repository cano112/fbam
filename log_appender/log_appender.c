#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <sys/param.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "../common/commons.h"

void append_log(FILE* log_file, file_access_message message)
{
    char log_message[MAX_LOG_LEN];
    snprintf(log_message, MAX_LOG_LEN, "%s, accessed: %s, %zu, %ld\n",
             message.function_name, message.file_path, message.count, message.offset);
    fputs(log_message, log_file);
    fflush(log_file);
}

void report_and_exit(FILE* log_file, const char* msg) {
    fclose(log_file);
    perror(msg);
    exit(-1); /* EXIT_FAILURE */
}

int main(int argc, char** argv)
{
    FILE *access_log_file = fopen(LOG_FILE_PATH, "a+");
    if (access_log_file != NULL)
    {
        key_t key = ftok(QUEUE_PATH_NAME, PROJECT_ID);
        if (key < 0) report_and_exit(access_log_file, "Key not gotten.");
        int queue_id = msgget(key, 0666 | IPC_CREAT);
        if (queue_id < 0) report_and_exit(access_log_file, "No access to queue.");
        while(1)
        {
            file_access_message message;
            if (msgrcv(queue_id, &message, sizeof(message), MESSAGE_TYPE, MSG_NOERROR) >=0)
            {
                append_log(access_log_file, message);
            }
            else
            {
                printf("Cannot receive message");
            }
        }
    }
}