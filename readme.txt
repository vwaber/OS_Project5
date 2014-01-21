Run Server first, then Client

prog5Server [-p <serverPort>]
prog5Client [-s <serverIP>] [-p <serverPort>]

Default server port is 1024. Attempts to use specified port, if not available checks for next available.
Default server IP is 0.0.0.0, which is the address of the current machine.

Client: accepts input from keyboard, sends to Server
Server: counts and categorizes all strings delimited by a white-space, sends to Client
Client: prints each string with category
Client: Exits upon receiving a blank line, sends exit message to server
Server: Prints summed total of each category recieved from server and exits

Type 0: ;)(   (Any one)
Type 1: *+-/  (Any one)
Type 2: numeric (Any length)
Type 3: All others, alphanumeric, combinations

Server operates using a main thread that loops checking for client connections.
Upon client connection, Server spawns new threads to handle client and resumes loop checking for new clients.
Both Server and Client process using multiple threads.
Queues are used to pass information between threads to prevent blocking.
Semaphores are used to ensure only one thread can access a queue at a given time.
Mutexes are used to insure threads are not interputed while accessing threads.
