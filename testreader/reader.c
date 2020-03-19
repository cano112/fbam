#include <unistd.h>
#include <stdio.h>

int main(int argc, char** argv) 
{
    char* test_file_path = argv[1];
    char buf[20];
    printf("Trying to read from path: %s\n", test_file_path);
 
    FILE *f = fopen(test_file_path, "r");
    int fd = fileno(f);
    size_t nbytes = sizeof(buf);
    read(fd, buf, nbytes);
    pread(fd, buf, nbytes - 5, 5);
}
