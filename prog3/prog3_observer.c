/* built off demo client lab */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#define TRUE  1
#define FALSE  0

/* function declarations */
char* get_username();

int main( int argc, char **argv) {
	struct hostent *ptrh; /* pointer to a host table entry */
	struct protoent *ptrp; /* pointer to a protocol table entry */
	struct sockaddr_in sad; /* structure to hold an IP address */
	int sd; /* socket descriptor */
	int port; /* protocol port number */
	char *host; /* pointer to host name */
	int n; /* number of characters read */
	char buf[1000]; /* buffer for data from the server */

	memset((char *)&sad,0,sizeof(sad)); /* clear sockaddr structure */
	sad.sin_family = AF_INET; /* set family to Internet */

	if( argc != 3 ) {
		fprintf(stderr,"Error: Wrong number of arguments\n");
		fprintf(stderr,"usage:\n");
		fprintf(stderr,"./client server_address server_port\n");
		exit(EXIT_FAILURE);
	}

	port = atoi(argv[2]); /* convert to binary */
	if (port > 0) /* test for legal value */
		sad.sin_port = htons((u_short)port);
	else {
		fprintf(stderr,"Error: bad port number %s\n",argv[2]);
		exit(EXIT_FAILURE);
	}

	host = argv[1]; /* if host argument specified */

	/* Convert host name to equivalent IP address and copy to sad. */
	ptrh = gethostbyname(host);
	if ( ptrh == NULL ) {
		fprintf(stderr,"Error: Invalid host: %s\n", host);
		exit(EXIT_FAILURE);
	}

	memcpy(&sad.sin_addr, ptrh->h_addr, ptrh->h_length);

	/* Map TCP transport protocol name to protocol number. */
	if ( ((long int)(ptrp = getprotobyname("tcp"))) == 0) {
		fprintf(stderr, "Error: Cannot map \"tcp\" to protocol number");
		exit(EXIT_FAILURE);
	}

	/* Create a socket. */
	sd = socket(PF_INET, SOCK_STREAM, ptrp->p_proto);
	if (sd < 0) {
		fprintf(stderr, "Error: Socket creation failed\n");
		exit(EXIT_FAILURE);
	}

	/* Connect the socket to the specified server. */
	if (connect(sd, (struct sockaddr *)&sad, sizeof(sad)) < 0) {
		fprintf(stderr,"connect failed\n");
		exit(EXIT_FAILURE);
	}

	/* Repeatedly read data from socket and write to user's screen. */
	//initial is determining if the connection can be established
	char connection_check;
	char name_check;

	recv(sd, &connection_check,1,0);
	if(connection_check == 'N'){
		printf("No room available on the server\n");
		close(sd);
		exit(EXIT_SUCCESS);
	}
//if we receive a 'Y' do get username
	while(name_check != 'Y'){
		char username [11];
		memset(username,0,sizeof(username));
		strcpy(username, get_username());
		//printf("after function we have: %s\n",username );
		uint8_t length = strlen(username);
		send(sd,&length,sizeof(length),0);
		send(sd,username,length,0);
		recv(sd, &name_check,1,0);
	}
	if(name_check == 'Y'){
		printf("Successfully connected to participant\n");
		char receivebuff[2048];
		while (1) {
			//print buffer
			memset(receivebuff,0,2048);
			uint16_t message_length;

			int readval = read(sd,&message_length,2);
			//so the observers don't get flooded when the server shuts down, not really important.
			if(readval ==0 ){
				break;
			}
			uint16_t converted = ntohs(message_length);
			read(sd, receivebuff,converted);
			receivebuff[converted] = '\0';
			printf("%s\n",receivebuff );
		}
	}

	close(sd);

	exit(EXIT_SUCCESS);
}


char* get_username(){
  int valid_name = FALSE;
  char* temp = malloc(100);
  int length = 0;
  while(valid_name == FALSE){
    printf("Enter a username: \n");
		memset(temp,0,sizeof(temp));
    scanf("%s",temp );
		length = strlen(temp);
    if(length < 11 && length > 0){
      valid_name = TRUE;
      char* returnchar = temp;
      return returnchar;
    }
  }
}
