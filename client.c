#include <stdio.h>
#include <stdlib.h>
#include <string.h>  // for strcmp() function
#include <unistd.h> // for close() function

#include <sys/socket.h> // socket functions
#include <sys/types.h> // socket types

#include <netinet/in.h> // socket address structures

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
    // create a socket
    int client_socket;
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        printf("Error creating socket\n");
        return 1;
    }
    // AF_INET is the address family for IPv4
    // SOCK_STREAM is the socket type for TCP
    // 0 is the protocol for TCP

    // specify an address for the socket
    struct sockaddr_in server_address; // sockaddr_in is a structure that contains the address of the server
    server_address.sin_family = AF_INET; // address family for IPv4
    server_address.sin_port = htons(6969); // port number in network byte order (host to network short)
    server_address.sin_addr.s_addr = INADDR_ANY; // IP address INADDR_ANY = 0.0.0.0

    // connect to the server
    int connection_status = connect(client_socket, (struct sockaddr *) &server_address, sizeof(server_address)); // connect to the server
    if (connection_status == -1) {
        printf("Error making a connection to the remote socket \n\n");
        close(client_socket);
        return 1;
    }
    
    printf("Connected to server! Starting communication...\n\n");
    
    // Receive initial message from server
    int message;
    int recv_result = receive_message(client_socket, &message);
    if (recv_result <= 0) {
        printf("Error receiving message from server\n");
        close_client(client_socket);
        return 1;
    }
    printf("The server sent: %d\n", message);
    
    // Communication loop
    while (1) {
        int message;
        receive_message(client_socket, &message);
        printf("The client received: %d\n", message);
        
        // sleep(1);
        
        send_message(client_socket, message + 1);
        printf("The client: %d\n", message + 1);
        
    }
    close_client(client_socket);
    return 0;
}