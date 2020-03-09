#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

//returns 0 if success, 1 if error
int readFile(char key[70000], char* filename)
{
    FILE* fp;
    fp = fopen(filename, "r");
    if(fp != NULL)
    {
        fscanf(fp, "%[^\n]", key);
    }
    else
    {
        fprintf(stderr, "Error-- could not open %s for reading.\n", filename);
        return 1;
    }
    
    int i;
    for(i=0;i<strlen(key);i++)
    {
        if( (key[i] < 'A' || key[i] > 'Z'))
        {
            if( key[i] != ' ')
            {
                return 1;
            }
        }    
    }
    return 0;
}

void error(const char *msg) { perror(msg); exit(0); } // Error function used for reporting issues

int main(int argc, char *argv[])
{
    char key[70000];
    char msg[70000];
    char send_text[140000];
    memset(key, '\0', sizeof(key));
    memset(msg, '\0', sizeof(msg));
    memset(send_text, '\0', sizeof(send_text));
  

	int socketFD, portNumber, charsWritten, charsRead;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char buffer[140000];
    
	if (argc != 4) { fprintf(stderr,"USAGE: %s plaintext key port\n", argv[0]); exit(0); } // Check usage & args
    
    //populate send_text variable with msg*key, check for error
    if( readFile(msg, argv[1]) || readFile(key, argv[2]))
    {
        fprintf(stderr, "Error -- bad characters in file.\n");
        return 1;
    }
    sprintf(send_text, "&%s*%s#", msg, key);


	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[3]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverHostInfo = gethostbyname("localhost"); // Convert the machine name into a special form of address
	if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

	// Set up the socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (socketFD < 0) error("CLIENT: ERROR opening socket");
	
	// Connect to server
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
		error("CLIENT: ERROR connecting");

	// Send message to server
	char* send_ptr = send_text;
    int bytesSent = 0;
	while(bytesSent < strlen(send_text))
	{
        charsWritten = send(socketFD, send_ptr, strlen(send_text), 0); // Write to the server
	    if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	    if (charsWritten < strlen(send_text))
        {
             //printf("CLIENT: WARNING: Not all data written to socket!\n");
             bytesSent += charsWritten;
             send_ptr += charsWritten;
        }
        else
        {
            //printf("CLIENT: all chars written\n");
            break;
        }
    }
	// Get return message from server
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
	charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
	if (charsRead < 0) error("CLIENT: ERROR reading from socket");
    if(buffer[0] != '*')
    {
	    printf("%s\n", buffer);
    }

	close(socketFD); // Close the socket
	return 0;
}
