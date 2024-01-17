#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <syslog.h>

#define PORT 9000
#define DATA_FILE "/var/tmp/aesdsocketdata"

int server_socket;

void handle_signal(int sig) {
    if (sig == SIGINT || sig == SIGTERM) {
        syslog(LOG_INFO, "Caught signal, exiting");
        close(server_socket);
        remove(DATA_FILE);
        closelog();
        exit(EXIT_SUCCESS);
    }
}

void handle_client(int client_socket) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[1024];

    getpeername(client_socket, (struct sockaddr*)&client_addr, &client_len);
    syslog(LOG_INFO, "Accepted connection from %s", inet_ntoa(client_addr.sin_addr));

    FILE* data_file = fopen(DATA_FILE, "a");
    if (data_file == NULL) {
        perror("Error opening data file");
        close(client_socket);
        return;
    }

    ssize_t bytes_received;
    while ((bytes_received = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
        buffer[bytes_received] = '\0';
        fprintf(data_file, "%s", buffer);

        // Check for newline to determine end of packet
        if (strchr(buffer, '\n') != NULL) {
            break;
        }
    }

    fclose(data_file);

    // Send the content of the data file back to the client
    data_file = fopen(DATA_FILE, "r");
    if (data_file == NULL) {
        perror("Error opening data file");
        close(client_socket);
        return;
    }

    while (fgets(buffer, sizeof(buffer), data_file) != NULL) {
        send(client_socket, buffer, strlen(buffer), 0);
    }

    fclose(data_file);
    close(client_socket);

    syslog(LOG_INFO, "Closed connection from %s", inet_ntoa(client_addr.sin_addr));
}

int main(int argc, char *argv[]) {
    if (argc > 1 && strcmp(argv[1], "-d") == 0) {
        // Daemon mode
        pid_t pid = fork();

        if (pid < 0) {
            perror("Error forking process");
            exit(EXIT_FAILURE);
        }

        if (pid > 0) {
            // Parent process in daemon mode
            exit(EXIT_SUCCESS);
        }

        // Child process continues
        setsid();
        chdir("/");
        umask(0);
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);
    }

    // Register signal handlers
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    openlog("aesdsocket", LOG_PID, LOG_USER);

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0) {
        perror("Error setting SO_REUSEADDR on  socket");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the socket to the specified port
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error binding socket");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, 5) == -1) {
        perror("Error listening for connections");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    syslog(LOG_INFO, "Server listening on port %d...", PORT);

    while (1) {
        // Accept a new connection
        int client_socket = accept(server_socket, NULL, NULL);
        if (client_socket == -1) {
            perror("Error accepting connection");
            continue;
        }

        // Create a new process to handle the client
        pid_t pid = fork();

        if (pid < 0) {
            perror("Error forking process");
            close(client_socket);
            continue;
        }

        if (pid == 0) {
            // Child process handles the client
            close(server_socket);
            handle_client(client_socket);
            exit(EXIT_SUCCESS);
        } else {
            // Parent process continues listening for new connections
            close(client_socket);
        }
    }

    return 0;
}

