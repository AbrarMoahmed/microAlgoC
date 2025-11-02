#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>

int main(){
	int sd, sc, c_id;
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

	recv(sd, &c_id, sizeof(c_id), 0);

	printf("My id is %d in the quee \n", c_id);

	// close the server :
	close(sd);
}
