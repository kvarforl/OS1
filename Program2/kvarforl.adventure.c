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

int main()
{
    char* room_dir = getMostRecentRoomDir(); 
    printf("Most recent dir: %s\n", room_dir);
    char* start_room = getStartRoom(room_dir);
    printf("Start: %s\n", start_room);
    return 0;
}
