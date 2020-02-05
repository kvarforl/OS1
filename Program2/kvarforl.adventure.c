#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>

char* getMostRecentRoomDir()
{
    DIR* this_dir = opendir(".");
    struct dirent* dptr;
    struct stat dstat;
    time_t most_recent = 0;
    char* champ;
    while( (dptr = readdir(this_dir)) != NULL)//iterate through contents of this directory (.)
    {
        if(dptr->d_type != DT_DIR || strcmp(dptr->d_name, ".") == 0 || strcmp(dptr->d_name, "..") == 0)
        {
            continue; //skip anything that isn't a directory, or is . or ..
        }
        
        if(stat(dptr->d_name, &dstat) != -1) //stat and check for error
        {
            if(dstat.st_mtime > most_recent) //compare timestamps
            {
                most_recent = dstat.st_mtime;//reset champ values if more recent
                champ = dptr->d_name;
            }
        }
        else{ printf("ERROR on stat %s\n", dptr->d_name); }
    }
    
    return champ;
}

char* getStartRoom(char* room_dir)
{
    DIR* this_dir = opendir(room_dir);
    struct dirent* dptr;
    struct stat dstat;
    time_t most_recent = 0;
    char* champ;
    while( (dptr = readdir(this_dir)) != NULL)//iterate through contents of this directory (.)
    {
        if(strcmp(dptr->d_name, ".") == 0 || strcmp(dptr->d_name, "..") == 0)
        {
            continue; //skip . or ..
        }
        char fpath[50];
        sprintf(fpath, "./%s/%s",room_dir, dptr->d_name);
        if(stat(fpath, &dstat) != -1) //stat and check for error
        {
            if(dstat.st_mtime > most_recent) //compare timestamps
            {
                most_recent = dstat.st_mtime;//reset champ values if more recent
                champ = dptr->d_name;
            }
        }
        else{ printf("ERROR on stat %s: %s\n", fpath, strerror(errno)); }
    }
    
    return champ;
}

void readFile(char* fpath, char* room_type)
{
    FILE* fp;
    fp = fopen(fpath, "r");
    char room[9];
    int counter = 0;
    while(fscanf(fp, "%*s %*s %s", room) == 1)
    {
        if(counter == 0)
        {
            printf("CURRENT LOCATION: %s\n", room);
        }
        else if(counter == 1)
        {
            printf("POSSIBLE CONNECTIONS: %s", room);
        }
        else if(strstr(room, "_ROOM") == NULL)
        {
            printf(", %s", room);
        }
        else
        {
            strcpy(room_type, room);
        } 
        counter += 1;
    }
    printf(".\n");
    fclose(fp);
}

void printRoom(char* room_name, char* dir_name, char conns[6][9], char* type)
{
    char fpath[50];
    sprintf(fpath, "./%s/%s", dir_name, room_name);
    
    FILE* fp;
    fp = fopen(fpath, "r");
    char room[9];
    int counter = 0;
    while(fscanf(fp, "%*s %*s %s", room) == 1)
    {
        if(counter == 0)
        {
            printf("CURRENT LOCATION: %s\n", room);
        }
        else if(counter == 1)
        {
            strcpy(conns[0], room); //populate conns with first connected room
            printf("POSSIBLE CONNECTIONS: %s", room);
        }
        else if(strstr(room, "_ROOM") == NULL)
        {
            strcpy(conns[counter-1], room); //counter is indexed at 1; we want it indexed at 0
            printf(", %s", room);
        }
        else
        {
            strcpy(type, room);
        } 
        counter += 1;
    }
    printf(".\n");
    fclose(fp);
}

//returns 1 if input is valid, 0 if not.
int getInput(char conns[6][9], char* response)
{
    int i;
    size_t res_size = 100;
    getline(&response, &res_size, stdin);
    response[strlen(response)-1] = '\0';//remove trailing newline char
    for(i=0;i<6;i++)
    {
        if(strcmp(response, conns[i]) == 0)
        {
            return 1;
        }
    }
    return 0;
}


int main()
{
    char* room_dir = getMostRecentRoomDir(); //const
    char* start_room = getStartRoom(room_dir); //const

    char room[9];
    char room_type[20];
    int step_count = 0;
    char path[200][9];//plz dont step more than 200 times; it will segfault. i know. u dont have that kind of time
    strcpy(room, start_room);//set start room

    do{
        char conns[6][9] ={{"null"}, {"null"}, {"null"}, {"null"}, {"null"}, {"null"}};//reset conns every room
        printRoom(room, room_dir, conns, room_type );
        if(strcmp(room_type, "END_ROOM") == 0)//check for win
        {
            break;
        }
        printf("WHERE TO? > ");

        char res[100];
        while(getInput(conns, res)== 0)
        {
            printf("\nHUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
            printRoom(room, room_dir, conns, room_type);
        }
        strcpy(path[step_count], res); //add room to path
        step_count += 1;//inc step count
        strcpy(room, res); //move to next room
    }
    while(1); //play game until end room is reached
     
    printf("\n\nYOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
    printf("YOU TOOK %i STEPS. YOUR PATH TO VICTORY WAS:\n", step_count);
    int i;
    printf("%s ", path[0]);
    for(i=1;i<step_count;i++)
    {
        printf("--> %s ", path[i]);
    }
    printf("\n");

    return 0;
}
