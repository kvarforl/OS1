#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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
        counter += 1; //post increment keeps counter as num elems
        token = strtok(NULL, delim);
    }
    return counter;
}

//changes directory to path; errors if invalid. DOES NOT SUPPORT ~. Does support .
void builtinCd(char tokens[512][40], int num_tokens)
{
    if(num_tokens == 1)
    {
        char* home = getenv("HOME");
        if (chdir(home) != 0)
        { 
            printf("Chdir to home failed\n");
        }
        printf("changed to home: %s\n", home);
    }
    else 
    {
        if (chdir(tokens[1]) != 0)
        {
            printf("Error: could not cd into %s\n", tokens[1]);
            fflush(stdout);
        }
    }
}


int main()
{
    char tokens[512][40];
    char input_str[2048];
    int shell_running = 1;    
 
    while(shell_running)//for testing 
    {
        int num_tokens = getTokenizedInput(input_str, tokens);
        //reprompt if blank or comment.
        if (num_tokens == 0 || tokens[0][0] == '#') //short circuit || to avoid seg fault
        {
            printf("comment or blank entered\n");
            continue;
        } 
        
        //check for builtins
        if(strcmp(tokens[0], "cd")==0)
        {
            printf("builtin cd\n");
            builtinCd(tokens, num_tokens);
        }
        else if(strcmp(tokens[0], "status")==0)
        {
            printf("builtin status\n");
        }
        else if(strcmp(tokens[0], "exit")==0)
        {
            printf("TODO: builtin exit\n");
            printf("just exiting while loop for now.\n");
            shell_running = 0;
        } 
        //fork and exec; let bash handle it. 
        else
        {    
            printf("Other cmd: %s; check other tokens.\n", tokens[0]);
        }
    }

    return 0;
}
