#include <stdio.h>
#include <stdlib.h>
#include <string.h>  // for strcmp() function
#include <unistd.h> // for close() function

#include <sys/socket.h> // socket functions
#include <sys/types.h> // socket types

#include <netinet/in.h> // socket address structures



void close_server(int server_socket) {
   close(server_socket); // close the server socket
   printf("Client exited the server \n\n");
}

void close_client(int client_socket) {
   close(client_socket); // close the client socket
   printf("Client disconnected from the server \n\n");
}

int send_message(int client_socket, int message) {
   return send(client_socket, &message, sizeof(message), 0);
}

int receive_message(int client_socket, int *message) {
   return recv(client_socket, message, sizeof(message), 0);
}

int main() {
   int server_socket;
   server_socket = socket(AF_INET, SOCK_STREAM, 0);
   if (server_socket == -1) {
      printf("Error creating socket\n");
      return 1;
   }
   // AF_INET is the address family for IPv4
   // SOCK_STREAM is the socket type for TCP
   // 0 is the protocol for TCP

   // Allow socket reuse to avoid "Address already in use" error
   int opt = 1;
   if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
      printf("Error setting socket options\n");
      close(server_socket);
      return 1;
   }

   //     (Template)    (Variable)
   struct sockaddr_in server_address; // sockaddr_in is a structure that contains the address of the server
   server_address.sin_family = AF_INET; // address family for IPv4
   server_address.sin_port = htons(6969); // port number in network byte order (host to network short)
   server_address.sin_addr.s_addr = INADDR_ANY; // IP address INADDR_ANY = 0.0.0.0

   // bind is a function that binds the socket to the address and port
   int bind_status = bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address));

   if (bind_status == -1) {
      printf("Error binding the socket to the address and port \n\n");
      close(server_socket);
      return 1;
   }
   
   printf("Server started on port 6969. Waiting for connections...\n\n");
   
   while (1) {
      // listen is a function that listens for incoming connections
      int listen_status = listen(server_socket, 5);
      if (listen_status == -1) {
         printf("Error listening for incoming connections \n\n");
         continue;
      }

      int client_socket;
      
      // accept is a function that accepts an incoming connection
      client_socket = accept(server_socket, NULL, NULL);
      if (client_socket == -1) {
         printf("Error accepting incoming connection \n\n");
         continue;
      }

      printf("Client connected! Starting communication...\n\n");
      
      // Send initial integer message to client
      int initial_message = 1;
      int send_status = send_message(client_socket, initial_message);
      if (send_status <= 0) {
         printf("Error sending initial message to the client \n\n");
         close_client(client_socket);
         continue;
      }
      printf("The server sent: %d\n", initial_message);
      
      // Communication loop with this client
      while (1) {
        int message;
        send_message(client_socket, message + 1);
        printf("The server: %d\n", message + 1);
        
      //   sleep(1);
        
        receive_message(client_socket, &message);
        printf("The server received: %d\n", message);
        
      }
   }
   close_server(server_socket);
   return 0;
}