# FBAM - File Block Access Monitoring Tool

Shared library used for logging file access operations in dynamically-linked Linux executables, build for HyperFlow WMS.

## Build

CMake 3.9.2 is used to build the library.

```bash
./build.sh
```

## Run
FBAM uses the following environment variables:
- ```HF_VAR_FS_MONIT_ENABLED``` - 1 enables, 0 disables monitoring
- ```HF_VAR_FS_MONIT_LOGFILE``` - path to the log file for writing monitoring logs
- ```HF_VAR_FS_MONIT_PATH_PATTERN``` - pattern of the files to include in file access monitoring logs

Set ```LD_PRELOAD``` environment variable to point to the FBAM library (*.so file) and execute dynamically linked exectable you want to monitor:

```bash
LD_PRELOAD=./build/libblockaccess.so.<FBAM version> <command>
```

## Sample test scripts

Shell script were prepared demonstrating how to use the library
The script builds Docker container with FBAM and runs simple executable with FBAM loaded through LD_PRELOAD environment variable.
Running test script:
```bash
./test.sh
```


