# C Projects

This repository contains three network programming projects implemented in C, demonstrating different aspects of client-server communication using TCP sockets.

## Projects Overview

1. **[Banking Queue](#bankingqueue)** - A queue management system that assigns numbers to clients
2. **[Group Chat](#groupchat)** - A multi-client chat server with real-time message broadcasting
3. **[Simple Messages](#simplemessages)** - A request-response messaging system between client and server

---

## BankingQueue

A simple queue management system that simulates a banking queue where clients connect to the server and receive their queue number.

### Features

- Sequential client handling
- Queue number assignment (1-10)
- Simple connection-based queue system

### How It Works

1. Client connects to the server
2. Server assigns a queue number to the client
3. Server sends the queue number to the client
4. Connection is closed
5. Process repeats for up to 10 clients

### Architecture

- **Server** (`server.c`): Listens on port 6969, accepts connections sequentially, assigns queue numbers
- **Client** (`client.c`): Connects to server and receives queue number

### Compilation

```bash
cd bankingQueue
gcc -o server server.c
gcc -o client client.c
```

### Usage

1. Start the server:
```bash
./server
```

2. Run clients (in separate terminals):
```bash
./client
```

Each client will receive their queue number upon connection.

---

## GroupChat

A multi-client chat application built with C, using TCP sockets and pthreads. This project demonstrates concurrent network programming where multiple clients can connect to a server and exchange messages in real-time.

### Features

- **Multi-client support**: Server can handle up to 100 concurrent clients
- **Real-time messaging**: Messages from one client are broadcast to all other connected clients
- **Thread-based architecture**: Each client connection is handled in a separate thread
- **Thread-safe operations**: Client list management uses mutex locks for concurrent access
- **Graceful disconnection**: Clients can disconnect cleanly with `/quit` or `/exit` commands

### Architecture

#### Server (`server.c`)
- Listens on port 6969 for incoming connections
- Uses `pthread` to handle each client in a separate thread
- Maintains a thread-safe list of all connected clients
- Broadcasts messages from one client to all others
- Automatically removes clients when they disconnect

#### Client (`client.c`)
- Connects to server on localhost (127.0.0.1) port 6969
- Uses separate thread for receiving messages while main thread handles sending
- Allows simultaneous sending and receiving of messages
- Supports graceful exit commands

### How It Works

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

### Port Explanation

#### Server Port (Fixed: 6969)
- The server listens on port **6969** for all incoming connections
- All clients connect to the same server port

#### Client Source Ports (Dynamic)
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

### Compilation

#### Requirements
- GCC compiler
- POSIX threads library (pthread)

#### Compile Server
```bash
cd groupChat
gcc -o server server.c -lpthread
```

#### Compile Client
```bash
gcc -o client client.c -lpthread
```

### Usage

#### 1. Start the Server
```bash
./server
```

You should see:
```
Server started on port 6969. Waiting for connections...
```

#### 2. Start Multiple Clients
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
Client 'Mohamed' connected (socket 4)
Client 'Mohamed' added. Total clients: 1
Thread started for client 'Mohamed' (socket 4)
New connection from 127.0.0.1:60884
Client 'yassine' connected (socket 5)
Client 'yassine' added. Total clients: 2
Thread started for client 'yassine' (socket 5)
'yassine' sent: Hello everyone!
'Mohamed' sent: Hi yassine!
```

**Terminal 2 (Client - Mohamed):**
```
Connecting to server...
Connected to server!
Enter your name: Mohamed
Welcome Mohamed! You are now connected.
Enter the message to send: Hi yassine!

yassine: Hello everyone!
Enter the message to send: 
```

**Terminal 3 (Client - yassine):**
```
Connecting to server...
Connected to server!
Enter your name: yassine
Welcome yassine! You are now connected.
Enter the message to send: Hello everyone!

Mohamed: Hi yassine!
Enter the message to send: 
```

### Code Structure

#### Server Functions

- `broadcast_message()`: Sends a message to all clients except the sender
- `add_client()`: Adds a new client to the client list (thread-safe)
- `remove_client()`: Removes a client from the list (thread-safe)
- `handle_client()`: Thread function that handles communication with one client
- `main()`: Sets up server socket and accepts connections

#### Client Functions

- `send_message()`: Sends a message to the server
- `receive_message()`: Receives a message from the server
- `receive_thread()`: Thread function that continuously receives messages
- `close_client()`: Closes connection and cleans up
- `main()`: Establishes connection and handles user input

### Thread Safety

The server uses `pthread_mutex_t` to ensure thread-safe access to the shared client list:
- `add_client()`: Protected by mutex
- `remove_client()`: Protected by mutex
- `broadcast_message()`: Protected by mutex

This prevents race conditions when multiple threads access the client list simultaneously.

---

## SimpleMessages

A simple request-response messaging system where clients send messages to the server and receive responses. The communication continues until the client sends "stop".

### Features

- Simple request-response pattern
- Interactive server responses
- Client-driven conversation flow
- Clean termination with "stop" command

### How It Works

1. Client connects to the server
2. Client sends a message to the server
3. Server receives and displays the message
4. Server prompts for a response
5. Server sends response back to client
6. Client displays the response
7. Process repeats until client sends "stop"

### Architecture

- **Server** (`server.c`): Listens on port 6969, handles one client at a time, sends responses based on user input
- **Client** (`client.c`): Connects to server, sends messages, receives and displays responses

### Compilation

```bash
cd simpleMessages
gcc -o server server.c
gcc -o client client.c
```

### Usage

1. Start the server:
```bash
./server
```

2. Run the client (in a separate terminal):
```bash
./client
```

3. Type messages in the client. The server will prompt for responses.
4. Type "stop" in the client to end the conversation.

### Example Session

**Terminal 1 (Server):**
```
server is up
server is binded
server is ready
Client connected
Message received: Hello
Enter your Answer:
Hi there!
Message received: How are you?
Enter your Answer:
I'm doing well!
Message received: stop
```

**Terminal 2 (Client):**
```
server is up
connected to the server
Enter message: Hello
Server response: Hi there!
Enter message: How are you?
Server response: I'm doing well!
Enter message: stop
```

---

## General Information

### Requirements

All projects require:
- GCC compiler
- POSIX-compliant system (Linux, macOS, or WSL)
- For GroupChat: pthread library (`-lpthread`)

### Port Usage

**Note**: All three projects use port **6969** by default. If you want to run multiple projects simultaneously, you'll need to modify the port numbers in the source code.

### Common Compilation Issues

- **Missing pthread library**: For GroupChat, ensure you link pthread with `-lpthread` flag
- **Port already in use**: Make sure no other process is using port 6969, or change the port number
- **Permission denied**: Some systems require elevated privileges for binding to ports < 1024

---

## Project Structure

```
c-projects/
├── README.md
├── bankingQueue/
│   ├── server.c
│   └── client.c
├── groupChat/
│   ├── server.c
│   └── client.c
└── simpleMessages/
    ├── server.c
    └── client.c
```

---
