# Multi-Client Chat Server

A simple multi-client chat application built with C, using TCP sockets and pthreads. This project demonstrates concurrent network programming where multiple clients can connect to a server and exchange messages in real-time.

## Features

- **Multi-client support**: Server can handle up to 100 concurrent clients
- **Real-time messaging**: Messages from one client are broadcast to all other connected clients
- **Thread-based architecture**: Each client connection is handled in a separate thread
- **Thread-safe operations**: Client list management uses mutex locks for concurrent access
- **Graceful disconnection**: Clients can disconnect cleanly with `/quit` or `/exit` commands

## Architecture

### Server (`server.c`)
- Listens on port 6969 for incoming connections
- Uses `pthread` to handle each client in a separate thread
- Maintains a thread-safe list of all connected clients
- Broadcasts messages from one client to all others
- Automatically removes clients when they disconnect

### Client (`client.c`)
- Connects to server on localhost (127.0.0.1) port 6969
- Uses separate thread for receiving messages while main thread handles sending
- Allows simultaneous sending and receiving of messages
- Supports graceful exit commands

## How It Works

1. **Connection Process**:
   - Client creates a socket and connects to server
   - Client sends their name to the server
   - Server adds client to its list and sends welcome message
   - Server creates a thread to handle this client's communication

2. **Message Broadcasting**:
   - Client sends a message to the server
   - Server receives the message in the client's thread
   - Server formats the message with sender's name
   - Server broadcasts the formatted message to all other clients
   - All other clients receive and display the message

3. **Disconnection**:
   - Client exits or connection is lost
   - Server detects disconnection in the client's thread
   - Server removes client from the list
   - Resources are cleaned up

## Port Explanation

### Server Port (Fixed: 6969)
- The server listens on port **6969** for all incoming connections
- All clients connect to the same server port

### Client Source Ports (Dynamic)
- Each client connection gets a **random ephemeral port** assigned by the OS
- Examples: 50962, 60884, 36978, 53076
- These are the client's **source ports**, not the server's port
- Different ports allow multiple clients from the same machine

**TCP Connection Structure:**
```
Client Side                    Server Side
----------                    -----------
IP: 127.0.0.1                IP: 127.0.0.1
Port: 50962 (random)  <--->   Port: 6969 (fixed)
```

This is standard TCP behavior - the server port is fixed, but each connection gets a unique client-side port.

## Compilation

### Requirements
- GCC compiler
- POSIX threads library (pthread)

### Compile Server
```bash
gcc -o server server.c -lpthread
```

### Compile Client
```bash
gcc -o client client.c -lpthread
```

## Usage

### 1. Start the Server
```bash
./server
```

You should see:
```
Server started on port 6969. Waiting for connections...
```

### 2. Start Multiple Clients
In separate terminal windows:

```bash
./client
```

For each client:
1. Enter your name when prompted
2. Start typing messages
3. Messages from other clients will appear automatically
4. Type `/quit` or `/exit` to disconnect

### Example Session

**Terminal 1 (Server):**
```
Server started on port 6969. Waiting for connections...
New connection from 127.0.0.1:50962
Client 'Alice' connected (socket 4)
Client 'Alice' added. Total clients: 1
Thread started for client 'Alice' (socket 4)
New connection from 127.0.0.1:60884
Client 'Bob' connected (socket 5)
Client 'Bob' added. Total clients: 2
Thread started for client 'Bob' (socket 5)
'Bob' sent: Hello everyone!
'Alice' sent: Hi Bob!
```

**Terminal 2 (Client - Alice):**
```
Connecting to server...
Connected to server!
Enter your name: Alice
Welcome Alice! You are now connected.
Enter the message to send: Hi Bob!

Bob: Hello everyone!
Enter the message to send: 
```

**Terminal 3 (Client - Bob):**
```
Connecting to server...
Connected to server!
Enter your name: Bob
Welcome Bob! You are now connected.
Enter the message to send: Hello everyone!

Alice: Hi Bob!
Enter the message to send: 
```

## Code Structure

### Server Functions

- `broadcast_message()`: Sends a message to all clients except the sender
- `add_client()`: Adds a new client to the client list (thread-safe)
- `remove_client()`: Removes a client from the list (thread-safe)
- `handle_client()`: Thread function that handles communication with one client
- `main()`: Sets up server socket and accepts connections

### Client Functions

- `send_message()`: Sends a message to the server
- `receive_message()`: Receives a message from the server
- `receive_thread()`: Thread function that continuously receives messages
- `close_client()`: Closes connection and cleans up
- `main()`: Establishes connection and handles user input

## Thread Safety

The server uses `pthread_mutex_t` to ensure thread-safe access to the shared client list:
- `add_client()`: Protected by mutex
- `remove_client()`: Protected by mutex
- `broadcast_message()`: Protected by mutex

This prevents race conditions when multiple threads access the client list simultaneously.
