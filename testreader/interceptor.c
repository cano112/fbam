#include <unistd.h>
#include <stdio.h>

int main(int argc, char** argv) 
{
    char *envp[] = {NULL};
    char *args[] = {"testreader", "./sample.txt", NULL};
    printf("First: %s", args[0]);
    execve("/fbam/build/testreader", args, envp);
}
