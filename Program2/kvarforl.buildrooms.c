#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//for dir creation
#include <sys/stat.h>

//for pid
#include <unistd.h>
#include <sys/types.h>


//checks contents of conn_counter
//returns 1 if all elems are at least 3
//returns 0 if any elem is less than 3
int isGraphFull( int* conn_counter)
{
    int i;
    for(i=0; i < 7; i++)
    {
        if(conn_counter[i] < 3)
        {
            return 0; //return false
        }
    }
    return 1;
}

//returns a random number between 0 and 6
int getRandomRoom()
{
    return rand() % 7;    
}

int main()
{
    srand(time(0));
    //create directory 
    char dirname[30];
    int pid = getpid();
    sprintf(dirname, "kvarforl.buildrooms.%i", pid);
    /*
    if(mkdir(dirname, 0755) == -1)
    {
        printf("ERROR - could not make directory\n");
    }
    */
    

    int connection_counter[7] = {0,0,0,0,0,0,0};

    return 0;
}


