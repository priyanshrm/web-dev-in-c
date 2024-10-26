#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <stdbool.h>
#define BUFFER_SIZE 1024
#define PORT_NUMBER 8080
#define URL "http://localhost:8080"

bool is_port_in_use(int port)
{
    struct addrinfo hints, *result;
    char port_str[6];

    // Convert port number to string
    sprintf(port_str, "%d", port);

    // Initialize hints
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    // Try to get address info
    if (getaddrinfo("localhost", port_str, &hints, &result) != 0)
    {
        return false;
    }

    // Try to create a socket and connect
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        freeaddrinfo(result);
        return false;
    }

    // Try to connect
    int status = connect(sock, result->ai_addr, result->ai_addrlen);

    // Clean up
    close(sock);
    freeaddrinfo(result);

    // If connection succeeded, port is in use
    return status == 0;
}

void open_browser()
{
    printf("Starting server at http://localhost:8080\n");
    printf("Waiting for server to initialize...\n");
    sleep(1); // Wait for 1 second

#ifdef __APPLE__
    printf("Opening browser on macOS...\n");
    system("open " URL);
#elif defined(__linux__)
    printf("Opening browser on Linux...\n");
    system("xdg-open " URL);
#elif defined(_WIN32)
    printf("Opening browser on Windows...\n");
    system("start " URL);
#endif

    printf("Browser launched! Server is ready to handle requests.\n");
}

int main(int argc, char const *argv[])
{
    if (is_port_in_use(PORT_NUMBER))
    {
        printf("Server already running on http://localhost:8080\n");
        return -1;
    }
    int web_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (web_socket == -1)
    {
        printf("Web Socket creation failed.\n");
        exit(0);
    }
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT_NUMBER);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(web_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        printf("ERROR! - binding web socket and server failure.\n");
        exit(0);
    }

    if (listen(web_socket, 10) == -1)
    {
        printf("ERROR! - listening for connections failure.\n");
        exit(0);
    }

    printf("Server started! Opening browser...\n");
    open_browser();
    printf("\n=== Server Log ===\n"); // This helps separate setup messages from connection logs

    struct sockaddr_in client_address;
    socklen_t client_length = sizeof(client_address);
    while (1)
    {
        int client_socket = accept(web_socket, (struct sockaddr *)&client_address, &client_length);
        if (client_socket == -1)
        {
            printf("ERROR! - accepting client connection failure.\n");
            continue; // continue listening for next connection
        }

        printf("New client connected!\n");
        char buffer[BUFFER_SIZE];
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);

        if (bytes_received == -1)
        {
            printf("ERROR! - receiving information from client failure.\n");
        }
        else if (bytes_received == 0)
        {
            printf("Client disconnected.\n");
        }
        else
        {
            buffer[bytes_received] = '\0'; // Null terminate the string
            printf("Received: %s\n", buffer);
        }
        // Create the HTTP response
        const char *http_response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: 48\r\n"
            "\r\n"
            "<html><body><h1>Hello from your web server!</h1></body></html>";

        // Send the HTTP response to the client
        ssize_t bytes_sent = send(client_socket, http_response, strlen(http_response), 0);
        if (bytes_sent < 0)
        {
            perror("send failed");
        }
        else
        {
            printf("HTTP response sent to client\n");
        }

        // Close the client and server sockets
        close(client_socket);
    }

    return 0;
}
