#include <stdio.h>

//for dir creation
#include <sys/stat.h>

//for pid
#include <unistd.h>
#include <sys/types.h>


int main()
{
    
    char dirname[30];

    int pid = getpid();
    sprintf(dirname, "kvarforl.buildrooms.%i", pid);
    
    if(mkdir(dirname, 0755) == -1)
    {
        printf("ERROR - could not make directory\n");
    }
    
    return 0;
}
