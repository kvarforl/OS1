#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

int fg_only_mode; // a global, to avoid signal passing to sig functions.
//returns the number of tokens gotten from input
//populates res (string arr) with each space delimited word of input
int getTokenizedInput(char* usr_input, char res[512][40]) 
{
    size_t input_size = 2048;
    char* token;
    char delim[2] = " ";

    printf(":");
    fflush(stdout);
    int num_chars = getline(&usr_input, &input_size, stdin);
    if(num_chars == -1)
    {
        clearerr(stdin);
    }
    usr_input[strlen(usr_input)-1] = '\0'; //remove trailing newline char
    
    //handle expansion of $$ to PID; add to readme that $$ is only expanded once
    char* expand_here = strstr(usr_input,"$$"); 
    if(expand_here != NULL)
    {
        char* first_half = usr_input;
        char second_half[50];
        strcpy(second_half,expand_here+2); 
        *expand_here = '\0';
        sprintf(usr_input, "%s%i%s",first_half,getpid(),second_half);
    }

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

//function that checks for file io in the last two tokens; should be called twice back to back
//returns 1 if error, 0 if not
int checkLastTwoTokens(char tokens[512][40], int* num_tokens, int* status)
{
    char* pos1 = tokens[(*num_tokens)-2]; //check 2nd to last token to be < or >
    char* fname1 = tokens[(*num_tokens)-1];//filename for inp or outp
        
    if(strcmp(pos1,"<") == 0 ) //check for input file
    {
        *num_tokens = (*num_tokens) - 2;
        int fd = open(fname1, O_RDONLY);
        if(fd != -1)
        {
            dup2(fd, STDIN_FILENO);
            return 0;
        }
        else
        {
            *status = 1;
            printf("Error -- could not open input file.\n");
            fflush(stdout);
            return 1;
        }
    }
    else if (strcmp(pos1, ">") == 0) //check for output file
    {
        *num_tokens = (*num_tokens) - 2;
        int fd = open(fname1, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if(fd != -1)
        {
            dup2(fd, STDOUT_FILENO);
            return 0;
        }
        else
        {
            *status = 1;
            printf("Error -- could not open output file.\n");
            fflush(stdout);
            return 1;
        }
    }
    return 0;
}


void execute(char tokens[512][40], int num_tokens, int* status)
{
    int fileStat1 = checkLastTwoTokens(tokens, &num_tokens, status);
    int fileStat2 = checkLastTwoTokens(tokens, &num_tokens, status);
    if(fileStat1 || fileStat2 )//exit if bad input or output files
    {
        exit(1);
    }    

    char* args[num_tokens];
    int i;
    for(i=0;i<num_tokens;i++)
    {
        args[i] = (char*) &tokens[i];
    }
    args[num_tokens] = NULL;
    if(execvp(args[0],args)< 0)
    {
        printf("Error -- exec failed! \n");
        exit(1);
    }
}

void cleanZombies(int* status)
{
    int exitMethod;
    pid_t dead = waitpid(-1, &exitMethod, WNOHANG);
    if(dead != 0 && dead != -1)
    {
        *status = WEXITSTATUS(exitMethod);
        printf("background process %i completed. ", dead );
        fflush(stdout);
        if(WIFSIGNALED(exitMethod) != 0)
        {
            printf("Killed by signal %i.", WTERMSIG(exitMethod));
            fflush(stdout);
        }
        else
        {
            printf("Exit status %i. ", *status);
        }
        printf("\n");
        fflush(stdout);
    }
}

//ignore and swipswap fg_only_mode
void catchSIGTSTP(int signo)
{
    if(fg_only_mode)
    {
        char* msg = "\nExiting foreground only mode.\n";
        write(STDOUT_FILENO, msg, 32);
    }
    else
    {
        char* msg = "\nEntering foreground only mode.\n";
        write(STDOUT_FILENO, msg, 32);
    }   
    fg_only_mode = !fg_only_mode;//switch the state of foreground only mode
}

int main()
{
    struct sigaction SIGTSTP_action = {0};
    SIGTSTP_action.sa_handler = catchSIGTSTP;
    sigfillset(&SIGTSTP_action.sa_mask);
    SIGTSTP_action.sa_flags = 0;
    sigaction(SIGTSTP, &SIGTSTP_action, NULL);

    //don't die on sigint (bg procs and parent)
    struct sigaction SIGINT_action = {0};
    SIGINT_action.sa_handler = SIG_IGN;
    sigfillset(&SIGINT_action.sa_mask);
    SIGINT_action.sa_flags = 0;
    sigaction(SIGINT, &SIGINT_action, NULL);

    char tokens[512][40];
    char input_str[2048];
    int shell_running = 1;    
    int status = 0;
    int run_in_background = 0;
    fg_only_mode = 0; // a global, to avoid signal passing to sig functions.

    while(shell_running) 
    {
        cleanZombies(&status);
        int num_tokens = getTokenizedInput(input_str, tokens);
        //reprompt if blank or comment.
        if (num_tokens == 0 || tokens[0][0] == '#') //short circuit || to avoid seg fault
        {
            //printf("comment or blank entered\n");
            continue;
        } 
        
        //check for builtins
        if(strcmp(tokens[0], "cd")==0)
        {
            builtinCd(tokens, num_tokens);
        }
        else if(strcmp(tokens[0], "status")==0)
        {
            printf("exit status %i\n", status);
        }
        else if(strcmp(tokens[0], "exit")==0)
        {
            shell_running = 0;
        } 
        //fork and exec 
        else
        {    
            //check for background
            run_in_background = 0;
            if (strcmp(tokens[num_tokens-1], "&") == 0)
            {
                num_tokens = num_tokens -1; //decrement num_tokens to omit trailing ampersand
                run_in_background = 1;
            }
            int childExitStatus;
            pid_t spawnPID = fork();
            switch(spawnPID)
            {
                case 0: //child process
                    
                    SIGTSTP_action.sa_handler = SIG_IGN;//ignore SIGTSTP in children
                    sigaction(SIGTSTP, &SIGTSTP_action, NULL);
                    if(run_in_background && !fg_only_mode)
                    {
                       printf("background process %i started\n:",getpid());
                       fflush(stdout);
                       int null = open("/dev/null", O_WRONLY);
                       dup2(null,1); //redirect stdout to null
                    }
                    else//process is in foreground; die on sigint
                    {
                        SIGINT_action.sa_handler = SIG_DFL;
                        sigaction(SIGINT, &SIGINT_action, NULL);
                    }
                    execute(tokens, num_tokens, &status);
                    break;
                default: //parent process
                    if(fg_only_mode || !run_in_background) //if not in background
                    {
                        waitpid(spawnPID, &childExitStatus, 0);
                        status = WEXITSTATUS(childExitStatus);
                    }
                    cleanZombies(&status);
                    break;
            }
        }
    }

    return 0;
}
