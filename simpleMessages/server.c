#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int send_mssg(int cnx, char[] mssg){
  return send(cnx, mssg, strlen(mssg), 0);
}

int recv_mssg(int cnx, char[] mssg){
  return recv(cnx, mssg, strlen(mssg), 0);
}


int main() {

  int sd, sb, sl, cnx, size;
  char req[250], res[250];
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

    while (1) {
      // resiving from  clients:
      size = recv(cnx, req, sizeof(req), 0);
      if (size > 0) {
        req[size] = '\0';
        printf("Message received: %s \n", req);
      }

      if (strcasecmp(req, "stop") == 0) {
        break;
      };

      // send res :
      printf("Enter your Answer : \n");
      int i = 0;
      while ((res[i] = getchar()) != '\n')
        i++;
      res[i] = '\0';
      send(cnx, res, strlen(res), 0);
    }
    // close connection :
    close(cnx);
  }
  // close the server:
  close(sd);
}
