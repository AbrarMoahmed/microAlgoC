#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>

int main(){
	int sd, sc, size;
	char req[256], res[256];
	struct sockaddr_in srvadr;
	sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd<0){
		printf("socket error \n");
		exit(0);
 	}else{
		printf("server is up \n");
	};

	// preparing the addresss
	srvadr.sin_family = AF_INET;
	srvadr.sin_port = 6969;
	srvadr.sin_addr.s_addr = INADDR_ANY;

	// connection :
	sc = connect(sd, (struct sockaddr *) &srvadr, sizeof(srvadr));
	if (sc < 0){
		printf("connection error \n");
		exit(0);
	}
		printf("connected to the server \n");
		
	do{
		//send :
		printf("enter ur mssg : \n");
		int i = 0;
		while((req[i] = getchar()) != '\n') i++;
		req[i] = '\0';

		// send the msg :
		send(sd, req, strlen(req), 0);

		if(strcasecmp(req, "stop") == 0) break;

		// repone from srv:
		if((size =  recv(sd, res, sizeof(res), 0)) > 0 ){
			res[size] = '\0';
			printf("response resived : %s \n", res);
		}
	} while(1);

	// close the server :
	close(sd);
}
