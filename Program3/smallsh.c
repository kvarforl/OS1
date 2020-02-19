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
    return counter +1;
}

int main()
{

    char tokens[512][40];
    char input_str[2048];
    int num_tokens = getTokenizedInput(input_str, tokens);
    
    int i;
    for(i=0;i<num_tokens;i++)
    {
        printf("%s\n", tokens[i]);
    }    

    return 0;
}
