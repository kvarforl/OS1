#include <string.h>
#include <stdio.h>


//returns the number of tokens gotten from input
//populates res (string arr) with each space delimited word of input
int getTokenizedInput(char* usr_input, char res[512][40]) 
{
    size_t input_size = 2048;
    char* token;
    char delim[2] = " ";

    printf(":");
    fflush(stdout);
    getline(&usr_input, &input_size, stdin);
    usr_input[strlen(usr_input)-1] = '\0'; //remove trailing newline char

    //populate result string array with tokenized input
    token = strtok(usr_input, delim);
    int counter = 0;
    while(token != NULL)
    {
        strcpy(res[counter], token);
        counter += 1;
        token = strtok(NULL, delim);
    }
    if(counter != 0) //want to return 0 (not 1) if no args provided
    {
        return counter +1;
    }
    return 0;
}

int main()
{

    char tokens[512][40];
    char input_str[2048];
    
    while(1)//for testing 
    {
        int num_tokens = getTokenizedInput(input_str, tokens);
        if (num_tokens == 0 || tokens[0][0] == '#') //short circuit || to avoid seg fault
        {
            printf("comment or blank entered\n");
            continue;
        } 
        
        if(strcmp(tokens[0], "cd")==0)
        {
            printf("builtin cd\n");
        }
        else if(strcmp(tokens[0], "status")==0)
        {
            printf("builtin status\n");
        }
        else if(strcmp(tokens[0], "exit")==0)
        {
            printf("builtin exit\n");
        } 
        else
        {    
            printf("Other cmd: %s; check other tokens.\n", tokens[0]);
        }
    }

    return 0;
}
