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

//returns 1 if room has less than 6 existing connections
//returns 0 if room has more than 6 existing connections
int canAddConnectionFrom(int room, int* conn_counter)
{
    if(conn_counter[room] < 6)
    {
        return 1;
    }
    return 0;
}

//returns 1 if y is already in x's adj list
//returns 0 if not
int connAlreadyExists(int x, int y, int conns[7][6])
{
    int i;
    for(i=0;i<6;i++)
    {
        if(conns[x][i] == y)
        {
            return 1;
        }
    }
    return 0;
}

//connects room x and y, bidirectionally
//updates adjacency list and connection counts
void connectRooms(int x, int y, int* conn_counter, int conns[][6])
{
    conns[x][conn_counter[x]] = y;
    conns[y][conn_counter[y]] = x;
    conn_counter[x] += 1;
    conn_counter[y] += 1;
}

//finds two rooms that are able to connect!
void addRandConn(int* conn_counter, int conns[][6])
{
    int A;
    int B;
    while(1)
    {
        A = getRandomRoom();
        if(canAddConnectionFrom(A, conn_counter))
        {
            break;
        }
    }

    do
    {
        B = getRandomRoom();
    }
    while(canAddConnectionFrom(B, conn_counter) == 0 || A == B || connAlreadyExists(A, B, conns) == 1);

    connectRooms(A,B, conn_counter, conns);
}

//for debugging /sanity only; prints adj list with room nums
void printAdjList(int* conn_counter, int conns[][6])
{

    int i;
    int j;
    for(i=0;i<7;i++)
    {
        printf("Room %i: ", i);
        for(j=0;j<conn_counter[i];j++)
        {
            printf("%i ", conns[i][j]);
        }
        printf("\n");
    }
    
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
    
    //create connection counter and adjacency list (with space for up to 6 cons)
    int connection_counter[7] = {0,0,0,0,0,0,0};
    int connections[7][6] = {
        {-1, -1, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1},
    };

    while(isGraphFull(connection_counter) == 0)
    {
        addRandConn(connection_counter, connections);
    }
    
    printAdjList(connection_counter, connections);
    
    return 0;
}


