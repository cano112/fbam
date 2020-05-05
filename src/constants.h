#ifndef FBAM_CONSTANTS_H
#define FBAM_CONSTANTS_H

#define TIMESTAMP_BUFFER_LEN 38

#define MAX_LOG_LEN (MAXPATHLEN + 50)
#define BUFFER_SIZE 100

#define HF_VAR_FS_MONIT_ENABLED "HF_VAR_FS_MONIT_ENABLED"
#define HF_VAR_FS_MONIT_COMMAND "HF_VAR_FS_MONIT_COMMAND"
#define HF_VAR_FS_MONIT_LOGFILE "HF_VAR_FS_MONIT_LOGFILE"
#define HF_VAR_FS_MONIT_PATH_PATTERN "HF_VAR_FS_MONIT_PATH_PATTERN"
#define LD_PRELOAD "LD_PRELOAD"

#define FUN_WRITE "write"
#define FUN_PWRITE "pwrite"
#define FUN_READ "read"
#define FUN_PREAD "pread"
#define FUN_OPEN "open"
#define FUN_CLOSE "close"
#define FUN_EXECVE "execve"
#define FUN_EXECVEAT "execveat"
#define FUN_FEXECVE "fexecve"
#define FUN_EXECLE "execle"
#define FUN_EXECVPE "execvpe"

#endif //FBAM_CONSTANTS_H
