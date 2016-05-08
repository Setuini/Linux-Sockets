// Posix, Berkley, BSD Socket Server
// Compile with gcc server.c -o server

#include "sys/socket.h"	// Core Socket Functions
#include "netinet/in.h"	// Adress and Protocol Families (AF_INET, AF_INET6, PF_INET, PF_INET6), TCP and UDP
#include "arpa/inet.h"	// Functions for manipulating numeric IP addresses
#include "netdb.h" 		// Functions for translating protocol names and host names into numeric addresses. Searches local data as well as name services.

#include "strings.h"	// bzero
#include "string.h"		// strcmp
#include "unistd.h"		// read write
#include "ctype.h"		// tolower
#include "stdlib.h"
#include "stdio.h"

char *msg_buffer_in, *msg_buffer_out;
int port, buffersize, n, socketfd, newsocketfd;
struct sockaddr_in server_adress, client_adress; // Structs for handling internet addresses (netinet/in.h)
socklen_t client_length;

int duplicateRequest[9];

void setupSockets();
void mainLoop();
void closeSockets();
char* deegreOfDanger(char* town);
void setDuplicateRequestZero();

int main(int argc, char** argv){

	port = 5000;
	buffersize = 256;

	setDuplicateRequestZero();
	setupSockets();
	mainLoop();

	closeSockets();

	return EXIT_SUCCESS;
}

void setupSockets(){
	// Open IPv4 Socket
	socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if (socketfd < 0){
		fprintf(stderr,"Error opening Socket\n");
		exit(0);
	}

	//set the server adress to zero (\0)
	bzero( (char*) &server_adress, sizeof(server_adress));
	// set the server adress to IPv4
	server_adress.sin_family = AF_INET;
	// set the port
	server_adress.sin_port = htons(port);
	//a socket bound with INADDR_ANY binds to the default IP address, which is that of the lowest-numbered interface
	server_adress.sin_addr.s_addr = INADDR_ANY;
	// alternative:
	//inet_aton("63.161.169.137", &server_adress.sin_addr);

	int tmp = bind(socketfd, (struct sockaddr *) &server_adress, sizeof(server_adress));
	if (tmp < 0){
		fprintf(stderr, "Error binding socket\n");
		exit(0);
	}

	// int listen(int sockfd, int backlog)
	// backlog defines the maximum length to which the queue of pending connections may grow
	listen(socketfd, 10);

	client_length = sizeof(client_adress);

	/* 
	*  Once you've gone through the trouble of getting a SOCK_STREAM socket and setting it up for incoming
	*  connections with listen(), then you call accept() to actually get yourself a new socket descriptor to
	*  use for subsequent communication with the newly connected client.
	*/
	newsocketfd = accept(socketfd, (struct sockaddr*) &client_adress, &client_length);
	if (newsocketfd < 0){
		fprintf(stderr, "Error on accept\n");
		exit(0);
	}
}

void mainLoop(){

	// allocate memory for the message buffers
	msg_buffer_in = (char*) malloc(sizeof(char)*buffersize);
	msg_buffer_out = (char*) malloc(sizeof(char)*buffersize);

	if(msg_buffer_out == NULL || msg_buffer_in == NULL){
		fprintf(stderr, "Error allocating memory\n");
		exit(0);
	}

	int running = 1;
	char* danger = (char*) malloc(sizeof(char));

	while(running){
		
		//clear the input buffer
		bzero(msg_buffer_in, buffersize);

		//Read from Socket
		n = read(newsocketfd, msg_buffer_in, buffersize-1);
		if (n < 0){
			fprintf(stderr, "Error reading from Socket\n");
			exit(0);
		}

		//remove line breaks from msg_buffer_in
		strtok(msg_buffer_in, "\n");
		//get level of danger as string
		danger = deegreOfDanger(msg_buffer_in);

		//clear the outputbuffer and fill with the new request
		bzero(msg_buffer_out, buffersize);
		strcpy(msg_buffer_out, "Dangerlevel for ");
		strcat(msg_buffer_out, msg_buffer_in);
		strcat(msg_buffer_out, " is ");
		strcat(msg_buffer_out, danger);

		// Write to socket
		n = write(newsocketfd, msg_buffer_out, buffersize);
		if (n < 0){
			fprintf(stderr, "Error writing to Socket\n");
			exit(0);
		}

		printf("message buffer in: %s\n",msg_buffer_in);
		printf("message buffer out: %s\n",msg_buffer_out);

	}
}

void closeSockets(){
	printf("closing sockets\n");
	close(newsocketfd);
	close(socketfd);
}

char* deegreOfDanger(char* town){

	//convert town to lowercase
	for(int i = 0; town[i]; i++){
	  town[i] = tolower(town[i]);
	}

	if(strcmp(town, "bregenz") == 0 && duplicateRequest[0] == 0){
		duplicateRequest[0] = 1;
		return "5";
	} else if(strcmp(town, "eisenstadt") == 0 && duplicateRequest[1] == 0){
		duplicateRequest[1] = 1;duplicateRequest[0] = 1;
		return "7";
	}else if(strcmp(town, "graz") == 0 && duplicateRequest[2] == 0){
		duplicateRequest[2] = 1;
		return "5";
	}else if(strcmp(town, "innsbruck") == 0 && duplicateRequest[3] == 0){
		duplicateRequest[3] = 1;
		return "2";
	}else if(strcmp(town, "klagenfurt") == 0 && duplicateRequest[4] == 0){
		duplicateRequest[4] = 1;
		return "7";
	}else if(strcmp(town, "linz") == 0 && duplicateRequest[5] == 0){
		duplicateRequest[5] = 1;
		return "7";
	}else if(strcmp(town, "salzburg") == 0 && duplicateRequest[6] == 0){
		duplicateRequest[6] = 1;
		return "3";
	} else if(strcmp(town, "stpoelten") == 0  && duplicateRequest[7] == 0){
		duplicateRequest[7] = 1;
		return "4";
	} else if(strcmp(town, "wien") == 0 && duplicateRequest[8] == 0){
		duplicateRequest[8] = 1;
		return "9";
	} else if(strcmp(town, "") == 0){
		return "";
	} else {
		printf("%s\n", "Invalid Access, shuting socket server down");
		closeSockets();
		exit(0);
	}
}

void setDuplicateRequestZero(){
	for (int i = 0; i < 0; i++){
		duplicateRequest[i] = 0;
	}
}
