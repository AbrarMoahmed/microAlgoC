#include<stdio.h>
#include<unistd.h>
#include<netdb.h>
#include<string.h>
#include<sys/socket.h>
#include<stdlib.h>

int main()
{
	int sd, sc, size;
	char req[1024];
	char fileName[10];
	struct sockaddr_in srvadr;
	FILE *f;

	memset(req, 0, sizeof(req));

	sd = socket(AF_INET, SOCK_STREAM, 0);
	if(sd<0) {printf("Socket Error");exit(0);}
	printf("Client Started.\n");

	// préparation de l'adresse
	srvadr.sin_family = AF_INET;
	srvadr.sin_port = 4321;
	srvadr.sin_addr.s_addr = INADDR_ANY;

	//connexion
	sc = connect(sd, (struct sockaddr *) &srvadr, sizeof(srvadr));
	if(sc<0) {printf("connect Error");exit(0);}
	printf("client connected.\n");

	// get the file name:
	printf("Enter le nom du fichier à envoyer.\n");
	int i = 0;
	while((fileName[i] = getchar()) != '\n') i++;
	fileName[i] = '\0';
	
	send(sd, fileName, strlen(fileName), 0); // send the file name 
	
	f = fopen(fileName, "r"); // open the file
	if(!f){ printf("File not found.\n"); close(sd); exit(0); } //prevent crash if file not found

	while( fgets(req, sizeof(req), f) != NULL) // getting the file from f and setting it in req 
	send(sd, req, strlen(req), 0); // sending the var req that hold file content
	printf("File sent.\n");
	
	send(sd, "EOF", strlen("EOF"), 0); // send the end of file 
	printf("EOF sent.\n");
	
	fclose(f);
	close(sd);

}
