#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

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
            fflush(stdout);
        }
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

void execute(char tokens[512][40], int num_tokens)
{
    char* args[num_tokens];
    int i;
    for(i=0;i<num_tokens;i++)
    {
        args[i] = (char*) &tokens[i];
    }
    args[num_tokens] = NULL;
    if(execvp(args[0],args)< 0)
    {
        printf("Exec failed! Exiting. \n");
        exit(1);
    }
}

int main()
{
    char tokens[512][40];
    char input_str[2048];
    int shell_running = 1;    
    int status = 0;

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
            printf("exit status %i\n", status);
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
            //printf("Other cmd: %s; check other tokens.\n", tokens[0]);
            int childExitStatus;
            pid_t spawnPID = fork();
            switch(spawnPID)
            {
                case 0:
                    //printf("In Child process: converting\n");
                    //fflush(stdout);
                    execute(tokens, num_tokens);
                    break;
                default:
                    //printf("PARENT(%d): Wait()ing for child(%d) to terminate\n", getpid(), spawnPID);
                    //pid_t actualPid = waitpid(spawnPID, &childExitStatus, 0);
                    waitpid(spawnPID, &childExitStatus, 0);
                    status = WEXITSTATUS(childExitStatus);
                    //printf("PARENT(%d): Child(%d) terminated, Exiting!\n", getpid(), actualPid);
                    break;
            }
        }
    }

    return 0;
}
