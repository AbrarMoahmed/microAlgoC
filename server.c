#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // for close() function

#include <sys/socket.h> // socket functions
#include <sys/types.h> // socket types

#include <netinet/in.h> // socket address structures


int main() {
   char server_message[256] = "You have reached the server!";
   int server_socket;
   server_socket = socket(AF_INET, SOCK_STREAM, 0);
   // AF_INET is the address family for IPv4
   // SOCK_STREAM is the socket type for TCP
   // 0 is the protocol for TCP
   struct sockaddr_in server_address; // sockaddr_in is a structure that contains the address of the server
   server_address.sin_family = AF_INET; // address family for IPv4
   server_address.sin_port = htons(6969); // port number in network byte order (host to network short)
   server_address.sin_addr.s_addr = INADDR_ANY; // IP address INADDR_ANY = 0.0.0.0

   // bind is a function that binds the socket to the address and port
   int bind_status = bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address));

   if (bind_status == -1) {
       printf("Error binding the socket to the address and port \n\n");
   }
   // listen is a function that listens for incoming connections
   int listen_status = listen(server_socket, 5);

   if (listen_status == -1) {
      printf("Error listening for incoming connections \n\n");
   }

   int client_socket;
   
   // accept is a function that accepts an incoming connection
   client_socket = accept(server_socket, NULL, NULL);

   if (client_socket == -1) {
      printf("Error accepting incoming connection \n\n");
   }

   // send is a function that sends data to the client  
   int send_status = send(client_socket, server_message, sizeof(server_message), 0);
   if (send_status == -1) {
      printf("Error sending data to the client \n\n");
   }

   // close the socket
   close(client_socket); // close the client socket
   close(server_socket); // close the server socket
   return 0;
   
}