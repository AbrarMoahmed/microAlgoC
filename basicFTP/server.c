#include<stdio.h>
#include<unistd.h>
#include<netdb.h>
#include<string.h>
#include<sys/socket.h>
#include<stdlib.h>
int main()
{
	int sd, sb, sl, cnx, size;
	char request[1024];
	struct sockaddr_in srvadr;
	char fileName[256];
	FILE *f;

	sd = socket(AF_INET, SOCK_STREAM, 0);
	if(sd<0) {printf("Socket Error");exit(0);}
	printf("Server Started.\n");

	// préparation de l'adresse
	srvadr.sin_family = AF_INET;
	srvadr.sin_port = 4321;
	srvadr.sin_addr.s_addr = INADDR_ANY;

	sb = bind(sd, (struct sockaddr *) &srvadr, sizeof(srvadr));
	if(sb<0) {printf("Bind Error");exit(0);}
	printf("Server Bined.\n");

	sl = listen(sd, 1);
	if(sl<0) {printf("listen Error");exit(0);}
	printf("Server ready.\n");

	cnx = accept(sd, 0, 0);
	if(cnx<0) {printf("Accept Error");exit(0);}
	printf("Client connected.\n");

	if((size = recv(cnx, request, sizeof(request), 0))>0){
		request[size]='\0';
		printf("File name received: %s\n", request);
	}
	
	strcpy(fileName, request);
	strcat(fileName, "1");
	
	f = fopen(fileName, "w");
	printf("File opned.\n");
	
	while(1)
	{
		if((size = recv(cnx, request, sizeof(request), 0))>0){
			request[size]='\0';
			
			if(strncmp(request, "EOF", 3) == 0) {
				printf("resiving EOF. \n");
				break;
			}
			fputs(request, f);
		}
	}

	fclose(f);
	printf("files closed. \n");
	close(cnx);
	printf("cnx closed. \n");
	close(sd);
	printf("server closed. \n");
}
