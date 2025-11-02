/**
 * Multi-Client Chat Client
 * 
 * This client program connects to a chat server and allows users to:
 * - Send messages to the server (which broadcasts to all clients)
 * - Receive messages from other clients in real-time
 * Uses threading to handle sending and receiving simultaneously.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>    // Socket functions (socket, connect, send, recv)
#include <sys/types.h>     // Socket types
#include <netinet/in.h>    // Internet address structures (sockaddr_in)
#include <arpa/inet.h>     // Internet address conversion functions (inet_addr)

#include <pthread.h>       // POSIX threads for concurrent message receiving

#define BUFFER_SIZE 1000   // Maximum message buffer size

// Global variables
int client_socket;         // Socket descriptor for connection to server
int is_connected = 1;      // Connection status flag (1 = connected, 0 = disconnected)

/**
 * Close client connection and update connection status
 * @param client_socket Socket descriptor to close
 */
void close_client(int client_socket) {
    is_connected = 0;                  // Mark as disconnected
    close(client_socket);              // Close the socket
    printf("Disconnected from the server\n");
}

/**
 * Send a message to the server
 * @param client_socket Socket descriptor for the server connection
 * @param message       Message string to send
 * @return Number of bytes sent, or -1 on error
 */
int send_message(int client_socket, char *message) {
    return send(client_socket, message, strlen(message), 0);
}

/**
 * Receive a message from the server
 * 
 * @param client_socket Socket descriptor for the server connection
 * @param message       Buffer to store received message
 * @param buffer_size   Size of the message buffer
 * @return Number of bytes received, or -1 on error
 */
int receive_message(int client_socket, char *message, int buffer_size) {
    // Reserve one byte for null terminator
    return recv(client_socket, message, buffer_size - 1, 0);
}

/**
 * Thread function to continuously receive messages from the server
 * This runs in a separate thread so we can receive messages while the
 * main thread handles user input for sending messages
 * @return NULL on thread exit
 */
void* receive_thread() {
    char received_message[BUFFER_SIZE];
    
    // Continuously receive messages while connected
    while (is_connected) {
        // Receive message from server (blocking call)
        int recv_result = receive_message(client_socket, received_message, sizeof(received_message));
        
        // Check for connection loss or error
        if (recv_result <= 0) {
            if (is_connected) {
                printf("\nConnection lost with server\n");
            }
            is_connected = 0;  // Update connection status
            break;
        }
        
        // Null-terminate the received message
        received_message[recv_result] = '\0';
        
        // Display the received message (from another client via server broadcast)
        printf("\n%s\n", received_message);
        printf("Enter the message to send: ");
        fflush(stdout);  // Force output to display immediately
    }
    
    pthread_exit(NULL);
}

int main() {
    // Create socket for TCP/IP communication
    // AF_INET: IPv4 address family
    // SOCK_STREAM: TCP (reliable, connection-oriented)
    // 0: Protocol (0 means default for SOCK_STREAM, which is TCP)
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        printf("Error creating socket\n");
        return 1;
    }
    
    // Configure server address structure
    // Server is running on localhost (127.0.0.1) on port 6969
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;                    // IPv4 address family
    server_address.sin_port = htons(6969);                  // Server port (convert to network byte order)
    server_address.sin_addr.s_addr = INADDR_ANY; // Server IP address (localhost)
    
    // Connect to the server
    printf("Connecting to server...\n");
    int connection_status = connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address));
    if (connection_status == -1) {
        printf("Error connecting to server\n");
        close(client_socket);
        return 1;
    }
    
    printf("Connected to server!\n\n");
    
    // Get client's display name from user
    char name[50];
    printf("Enter your name: ");
    if (fgets(name, sizeof(name), stdin) == NULL) {
        printf("Error reading name\n");
        close_client(client_socket);
        return 1;
    }
    // Remove newline character from input
    name[strcspn(name, "\n")] = 0;
    
    // Send name to server immediately after connection
    if (send(client_socket, name, strlen(name), 0) <= 0) {
        printf("Error sending name to server\n");
        close_client(client_socket);
        return 1;
    }
    
    // Receive welcome message from server
    char welcome_msg[BUFFER_SIZE];
    int welcome_bytes = recv(client_socket, welcome_msg, sizeof(welcome_msg) - 1, 0);
    if (welcome_bytes > 0) {
        welcome_msg[welcome_bytes] = '\0';  // Null-terminate
        printf("%s", welcome_msg);          // Display welcome message
    }
    
    // Create a separate thread for receiving messages
    // This allows us to receive messages concurrently while sending messages
    pthread_t recv_thread;
    if (pthread_create(&recv_thread, NULL, receive_thread, NULL) != 0) {
        printf("Error creating receive thread\n");
        close_client(client_socket);
        return 1;
    }
    
    // Main loop: handle user input and send messages
    char message[BUFFER_SIZE];
    printf("Enter the message to send: ");
    
    while (is_connected) {
        // Read message from user input
        if (fgets(message, sizeof(message), stdin) == NULL) {
            break;  // EOF or error
        }
        
        // Remove newline character from input
        message[strcspn(message, "\n")] = 0;
        
        // Check if user wants to quit
        if (strcmp(message, "/quit") == 0 || strcmp(message, "/exit") == 0) {
            printf("Disconnecting...\n");
            break;
        }
        
        // Skip empty messages
        if (strlen(message) == 0) {
            printf("Enter the message to send: ");
            continue;
        }
        
        // Send message to server
        if (send_message(client_socket, message) < 0) {
            printf("Error sending message to server\n");
            break;
        }
        
        // Prompt for next message
        printf("Enter the message to send: ");
    }
    
    // Cleanup: gracefully shutdown connection
    is_connected = 0;                    // Signal receive thread to exit
    pthread_cancel(recv_thread);        // Cancel receive thread
    pthread_join(recv_thread, NULL);    // Wait for thread to finish
    close_client(client_socket);        // Close socket
    
    return 0;
}