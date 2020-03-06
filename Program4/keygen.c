#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char** argv)
{
    if(argc != 2) //check cmd line args
    {
        printf("Usage: ./keygen <number_of_chars>\n");
        return 1;
    }

    srand(time(0));    

    int num_chars;
    sscanf(argv[1], "%d",&num_chars);
    
    char key[num_chars];
    key[num_chars] = '\0';

    int i;
    for(i=0;i<num_chars;i++)
    {
       int r = rand() % 27 + 65;
       if(r == 91)
       {
            r = 32; //change [ to space
       } 
       key[i] = r;
    }
    
    printf("%s\n", key);   
  

    return 0;
}
