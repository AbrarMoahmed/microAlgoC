#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // for close() function

#include <sys/socket.h> // socket functions
#include <sys/types.h> // socket types

#include <netinet/in.h> // socket address structures


int main() {
    // create a socket
    int client_socket;
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    // AF_INET is the address family for IPv4
    // SOCK_STREAM is the socket type for TCP
    // 0 is the protocol for TCP

    // sepify an address for the socket
    struct sockaddr_in server_address; // sockaddr_in is a structure that contains the address of the server
    server_address.sin_family = AF_INET; // address family for IPv4
    server_address.sin_port = htons(6969); // port number in network byte order (host to network short)
    server_address.sin_addr.s_addr = INADDR_ANY; // IP address INADDR_ANY = 0.0.0.0
    
    // connect to the server
    int connection_status = connect(client_socket, (struct sockaddr *) &server_address, sizeof(server_address)); // connect to the server
    if (connection_status == -1) {
        printf("Error making a connection to the remote socket \n\n");
    }

    // receive data from the server
    char server_response[256]; // server_response is a buffer that will store the data received from the server
    recv(client_socket, &server_response, sizeof(server_response), 0); // recv is a function that receives data from the server
    printf("The server sent the data: %s\n", server_response);

    // close the socket
    close(client_socket); // close the socket
    return 0;
}