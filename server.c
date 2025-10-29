/**
 * Multi-Client Chat Server
 * 
 * This server program accepts multiple client connections and broadcasts
 * messages from one client to all other connected clients, creating a
 * simple chat room functionality.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <sys/socket.h>    // Socket functions (socket, bind, listen, accept, send, recv)
#include <sys/types.h>     // Socket types
#include <netinet/in.h>    // Internet address structures (sockaddr_in)
#include <arpa/inet.h>     // Internet address conversion functions (inet_ntoa)

#include <pthread.h>       // POSIX threads for handling multiple clients concurrently

// Constants
#define MAX_CLIENTS 100    // Maximum number of concurrent clients
#define BUFFER_SIZE 1000   // Maximum message buffer size
#define PORT 6969          // Server listening port

/**
 * Client information structure
 * Stores socket descriptor, client name, and unique ID for each connected client
 */
typedef struct {
    int socket;            // Client's socket file descriptor
    char name[50];         // Client's display name
    int id;                // Unique client identifier
} client_info_t;

// Global variables for client management
client_info_t clients[MAX_CLIENTS];           // Array to store all connected clients
int client_count = 0;                          // Current number of connected clients
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;  // Mutex for thread-safe client list access

/**
 * Broadcast a message to all connected clients except the sender
 * 
 * @param message       The message content to broadcast
 * @param sender_name   Name of the client who sent the message
 * @param sender_socket Socket descriptor of the sender (to exclude from broadcast)
 */
void broadcast_message(char *message, char *sender_name, int sender_socket) {
    // Lock mutex to ensure thread-safe access to client list
    pthread_mutex_lock(&clients_mutex);
    
    // Format message with sender's name (e.g., "client1: hello")
    char formatted_message[BUFFER_SIZE + 100];
    snprintf(formatted_message, sizeof(formatted_message), "%s: %s", sender_name, message);
    
    // Send formatted message to all clients except the sender
    for (int i = 0; i < client_count; i++) {
        // Skip the sender and invalid sockets
        if (clients[i].socket != sender_socket && clients[i].socket > 0) {
            send(clients[i].socket, formatted_message, strlen(formatted_message), 0);
        }
    }
    
    // Unlock mutex after accessing client list
    pthread_mutex_unlock(&clients_mutex);
}

/**
 * Remove a client from the client list when they disconnect
 * @param client_socket Socket descriptor of the client to remove
 */
void remove_client(int client_socket) {
    // Lock mutex for thread-safe removal
    pthread_mutex_lock(&clients_mutex);
    
    // Find the client in the list
    for (int i = 0; i < client_count; i++) {
        if (clients[i].socket == client_socket) {
            // Shift all clients after this one up to fill the gap
            for (int j = i; j < client_count - 1; j++) {
                clients[j] = clients[j + 1];
            }
            client_count--;  // Decrease client count
            break;
        }
    }
    
    pthread_mutex_unlock(&clients_mutex);
}

/**
 * Add a new client to the client list
 * @param client_socket Socket descriptor of the new client
 * @param name          Display name of the new client
 */
void add_client(int client_socket, char *name) {
    // Lock mutex for thread-safe addition
    pthread_mutex_lock(&clients_mutex);
    
    // Check if we have room for more clients
    if (client_count < MAX_CLIENTS) {
        // Add client to the end of the list
        clients[client_count].socket = client_socket;
        clients[client_count].id = client_count;
        // Safely copy name with null-termination
        strncpy(clients[client_count].name, name, sizeof(clients[client_count].name) - 1);
        clients[client_count].name[sizeof(clients[client_count].name) - 1] = '\0';
        client_count++;
        printf("Client '%s' added. Total clients: %d\n", name, client_count);
    }
    
    pthread_mutex_unlock(&clients_mutex);
}

/**
 * Thread function to handle communication with a single client
 * Each client connection runs in its own thread
 * @param arg Pointer to the client_info_t structure for this client
 * @return NULL on thread exit
 */
void* handle_client(void* arg) {
    // Extract client information from argument
    client_info_t *client = (client_info_t*)arg;
    int client_socket = client->socket;
    char client_name[50];
    strncpy(client_name, client->name, sizeof(client_name));
    
    // Buffer to receive messages from client
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    
    printf("Thread started for client '%s' (socket %d)\n", client_name, client_socket);
    
    // Main loop: continuously receive messages from this client
    while (1) {
        // Receive message from client (blocking call)
        bytes_read = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        
        // Check if client disconnected or error occurred
        if (bytes_read <= 0) {
            // Client disconnected
            printf("Client '%s' disconnected\n", client_name);
            remove_client(client_socket);  // Remove from client list
            close(client_socket);           // Close the socket
            break;
        }
        
        // Null-terminate the received message
        buffer[bytes_read] = '\0';
        
        // Display message on server console
        printf("'%s' sent: %s\n", client_name, buffer);
        
        // Broadcast message to all other connected clients
        broadcast_message(buffer, client_name, client_socket);
    }
    
    // Exit thread
    pthread_exit(NULL);
}

int main() {
    int server_socket;                    // Main server socket descriptor
    struct sockaddr_in server_address;    // Server address structure
    int opt = 1;                          // Option for setsockopt
    
    // Create socket for TCP/IP communication
    // AF_INET: IPv4 address family
    // SOCK_STREAM: TCP (reliable, connection-oriented)
    // 0: Protocol (0 means default for SOCK_STREAM, which is TCP)
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        printf("Error creating socket\n");
        return 1;
    }
    
    // Set socket option to allow address reuse
    // This prevents "Address already in use" error when restarting server quickly
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        printf("Error setting socket options\n");
        close(server_socket);
        return 1;
    }
    
    // Configure server address structure
    server_address.sin_family = AF_INET;          // IPv4 address family
    server_address.sin_port = htons(PORT);        // Convert port to network byte order
    server_address.sin_addr.s_addr = INADDR_ANY;  // Listen on all available interfaces (0.0.0.0)
    
    // Bind socket to server address and port
    // This associates the socket with the server's IP address and port number
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        printf("Error binding the socket to the address and port\n");
        close(server_socket);
        return 1;
    }
    
    // Start listening for incoming connections
    // 5: Maximum length of the queue of pending connections
    if (listen(server_socket, 5) < 0) {
        printf("Error listening for incoming connections\n");
        close(server_socket);
        return 1;
    }
    
    printf("Server started on port %d. Waiting for connections...\n\n", PORT);
    
    // Initialize client count
    client_count = 0;
    
    // Main server loop: accept connections indefinitely
    while (1) {
        struct sockaddr_in client_address;    // Client address structure
        socklen_t client_addr_len = sizeof(client_address);
        int client_socket;                    // Socket for individual client connection
        
        // Accept an incoming connection
        // This blocks until a client connects
        // client_address is filled with client's IP and port information
        client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_addr_len);
        if (client_socket < 0) {
            printf("Error accepting incoming connection\n");
            continue;  // Continue waiting for next connection
        }
        
        // Display connection information
        // inet_ntoa: Convert IP address to string
        // ntohs: Convert port from network byte order to host byte order
        printf("New connection from %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
        
        // Receive client's name (sent immediately after connection)
        char client_name[50];
        ssize_t name_bytes = recv(client_socket, client_name, sizeof(client_name) - 1, 0);
        if (name_bytes <= 0) {
            printf("Error receiving client name\n");
            close(client_socket);
            continue;
        }
        client_name[name_bytes] = '\0';  // Null-terminate the name
        
        printf("Client '%s' connected (socket %d)\n", client_name, client_socket);
        
        // Add client to the client list
        add_client(client_socket, client_name);
        
        // Send welcome message to the new client
        char welcome_msg[100];
        snprintf(welcome_msg, sizeof(welcome_msg), "Welcome %s! You are now connected.\n", client_name);
        send(client_socket, welcome_msg, strlen(welcome_msg), 0);
        
        // Create a new thread to handle this client's communication
        // Each client runs in its own thread, allowing concurrent communication
        client_info_t *new_client = &clients[client_count - 1];
        pthread_t client_thread;
        
        if (pthread_create(&client_thread, NULL, handle_client, (void*)new_client) != 0) {
            printf("Error creating thread for client\n");
            remove_client(client_socket);
            close(client_socket);
            continue;
        }
        
        // Detach thread so it cleans up automatically when it exits
        // We don't need to join this thread later
        pthread_detach(client_thread);
    }
    
    // This code never reached, but included for completeness
    close(server_socket);
    return 0;
}