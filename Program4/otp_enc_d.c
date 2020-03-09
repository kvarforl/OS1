#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

int mod(int num, int m)
{
    int res;
    if(num >= 0)
    {
        res = num % m;
    }
    else
    {
        res = num + m;
    }
    return res;
}

void encrypt(char* text, char* key, char res[70000])
{
    if(strlen(text) > strlen(key))
    {
        fprintf(stderr, "Error--key length (%zu) is shorter than message length (%zu).\n", strlen(key), strlen(text));
        //exit(1);
        return;
    }
    
    int num_chars = strlen(text);
    int i;
    for(i=0;i<num_chars;i++)
    {
        int t = text[i] - 'A';
        int k = key[i] - 'A';

        if( t < 0) t = 26;
        if( k < 0) k = 26;

        int num = t+k;
        num = mod(num, 27);
        if( num == 26)
        {
            res[i] = ' ';
        }
        else
        {
            res[i] = num + 65;  
        }
    }
}


void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues

int main(int argc, char *argv[])
{
	int listenSocketFD, establishedConnectionFD, portNumber, charsRead;
	socklen_t sizeOfClientInfo;
	char buffer[140000]; //big enough to hold both 70000 char msg and key
    char res[70000];
	struct sockaddr_in serverAddress, clientAddress;

	if (argc < 2) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); } // Check usage & args

	// Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

	// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0) error("ERROR opening socket");

	// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
		error("ERROR on binding");
	listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections

    while(1)
    {
	    // Accept a connection, blocking if one is not available until one connects
	    sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
	    establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
	    if (establishedConnectionFD < 0) error("ERROR on accept");

        pid_t spawnPID = fork();
        switch(spawnPID)
        {
            case 0://child process
  
 
                //break message from key on * pattern, and end message with #
	            memset(buffer, '\0', 140000);
                char* ptr = buffer;
                while(1)
                {
	                charsRead = recv(establishedConnectionFD, ptr, 139999, 0); // Read the client's message from the socket
	                if (charsRead < 0) error("ERROR reading from socket");
                    if(ptr[charsRead-1] != '#')
                    {
                        //printf("term # not received.\n");
                        ptr += charsRead;
                    }
                    else
                    {
                        //printf("Found end #\n");
                        break;
                    }
                }
                memset(res, '\0', sizeof(res));
                res[0] = '*';
                if(buffer[0] != '&') //& is symbol coming from enc
                {
                    fprintf(stderr, "Error -- only otp_enc can connect to opt_enc_d.\n");
                    //if it's not from enc, stop;
                } 
                else
                {
                    buffer[strlen(buffer)-1] = '\0'; //get rid of end #
                    char* start_ptr = buffer + 1; //move start pointer forward one to ignore beginning &
                    //split into message and key
                    char* split = strstr(buffer, "*");
                    if(split != NULL)
                    {
                        split[0] = '\0';
                        encrypt(start_ptr, split+1, res);
                    }
                    else{ printf("MUST INCLUDE * TO SPLIT MSG AND KEY\n");}
                }
                
                // Send a Success message back to the client
	            charsRead = send(establishedConnectionFD, res, strlen(res), 0); // Send success back
	            if (charsRead < 0) error("ERROR writing to socket");
                close(establishedConnectionFD); // Close the existing socket which is connected to the client
                break;
        
            default://parent; keeps listening
                break;
        }
    }
	close(listenSocketFD); // Close the listening socket
	return 0; 
}
