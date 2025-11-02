#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>


int main() {

  int sd, sb, sl, cnx, client_number;
  struct sockaddr_in srvadr;
  sd = socket(AF_INET, SOCK_STREAM, 0);
  if (sd < 0) {
    printf("socket error \n");
    exit(0);
  } else {
    printf("server is up \n");
  }

  // prepare the addrs :
  srvadr.sin_family = AF_INET;
  srvadr.sin_port = 6969;
  srvadr.sin_addr.s_addr = INADDR_ANY;

  // binding :
  sb = bind(sd, (struct sockaddr *)&srvadr, sizeof(srvadr));
  if (sd < 0) {
    printf("binding error \n");
    exit(0);
  }
  printf("server is binded \n");

  // listining :
  sl = listen(sd, 1);
  if (sl < 0) {
    printf("listen error \n");
    exit(0);
  }
  printf("server is ready \n");

  // conecting :
  while (1) {

    cnx = accept(sd, 0, 0);
    if (cnx < 0) {
      printf("Accept Error \n");
      exit(0);
    }
    printf("Client connected \n");
    
    client_number++;

    printf("Client number: %d \n", client_number);

    // send the client number to the client :
    send(cnx, &client_number, sizeof(client_number), 0);

    if(client_number == 10) break;

    // close connection :
    close(cnx);
  }
  // close the server:
  close(sd);
}
