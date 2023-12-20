#include <arpa/inet.h>
#include <fcntl.h>
#include <getopt.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_CLIENTS 5
#define BUFFER_SIZE 1024
#define USERNAME_SIZE 32

const char *hello_client = "This is brand new 4chan.\n";

typedef struct {
    int socket;
    char username[USERNAME_SIZE];
} Client;

typedef struct {
    Client clients[MAX_CLIENTS];
    int client_count;
    pthread_mutex_t mutex;
} ClientList;

typedef struct
{
    ClientList *client_list;
    int client_index;
} thread_info;

void handle_client(thread_info *info);
void broadcast(ClientList *client_list, char *message,
               const int client_index, int size);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <ip_address> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *ip_address = argv[1];
    int port = atoi(argv[2]);

    // Create socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Bind to address and port
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(ip_address);
    server_address.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr *)&server_address,
             sizeof(server_address)) == -1) {
        perror("Error binding socket");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, MAX_CLIENTS) == -1) {
        perror("Error listening");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on %s:%d\n", ip_address, port);

    ClientList client_list;
    client_list.client_count = 0;
    pthread_mutex_init(&client_list.mutex, NULL);

    while (1) {
        // Accept a connection
        struct sockaddr_in client_address;
        socklen_t client_addr_size = sizeof(client_address);
        int client_socket = accept(server_socket,
                                   (struct sockaddr *)&client_address,
                                   &client_addr_size);
        if (client_socket == -1) {
            perror("Error accepting connection");
            continue;
        }

        printf("Connection accepted from %s:%d\n",
               inet_ntoa(client_address.sin_addr),
               ntohs(client_address.sin_port));

        // Receive the username from the client
        send(client_socket, hello_client, strlen(hello_client), 0);

        char username[USERNAME_SIZE];
        int bytes_received = recv(client_socket, username, sizeof(username), 0);
        if (bytes_received <= 0) {
            printf("Client disconnected\n");
            close(client_socket);
            continue;
        }

        // Null-terminate the received username
        username[bytes_received] = '\0';

        // Add the client to the array
        if (client_list.client_count < MAX_CLIENTS) {
            Client new_client;
            int current_client;
            new_client.socket = client_socket;
            strncpy(new_client.username, username, sizeof(new_client.username));

            // Add the new client to the array
            current_client = client_list.client_count;
            client_list.clients[client_list.client_count] = new_client;
            client_list.client_count++;

            // Create a new thread to handle the client
            thread_info info;
            info.client_index = current_client;
            info.client_list = &client_list;
            pthread_t client_thread;
            if (pthread_create(&client_thread, NULL,
                               (void *(*)(void *))handle_client, &info) != 0) {
                perror("Error creating thread");
                close(client_socket);
                pthread_mutex_unlock(&client_list.mutex);
                continue;
            }

            // Detach the thread, as we won't be joining it
            pthread_detach(client_thread);
        } else {
            printf("Server is full. Connection rejected.\n");
            close(client_socket);
        }
    }

    // Cleanup
    pthread_mutex_destroy(&client_list.mutex);
    close(server_socket);

    return 0;
}

void handle_client(thread_info *info) {
    const int client_index = info->client_index;
    ClientList *client_list;
    client_list = info->client_list;

    char buffer[BUFFER_SIZE];
    int bytes_received;

    while (1) {
        // Receive data from the client
        bytes_received = recv(client_list->clients[client_index].socket,
                              buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
            printf("Client %s disconnected\n",
                   client_list->clients[client_index].username);
            break;
        }

        // Null-terminate the received data
        buffer[bytes_received] = '\0';

        // Handle command (you can extend this part for handling various commands)
        if (strcmp(buffer, "/exit") == 0) {
            printf("Client %s requested exit\n",
                   client_list->clients[client_index].username);
            break;
        } else if (strncmp(buffer, "/file", 5) == 0) {
            broadcast(client_list, buffer, client_index, bytes_received);
            int size = 0;
            recv(client_list->clients[client_index].socket,
                 &size, sizeof(int), 0);
            printf("Size: %d\n", size);
            broadcast(client_list, &size, client_index, sizeof(int));
            char *buffer_1 = malloc(size * sizeof(char));
            int bytes_received;
            int length = 0;

            while (length < size) {
                // Receive data from the server
                bytes_received = recv(client_list->clients[client_index].socket, buffer_1 + length, size - length + 1, 0);
                if (bytes_received <= 0) {
                    free(buffer_1);
                    break;
                }

                length += bytes_received;

                // Write the received data to the file
            }
            broadcast(client_list, buffer_1, client_index, size);

            printf("%s\n", buffer_1);

            free(buffer_1);
        } else {
            // Print the received message
            printf("Received from %s: %s\n",
                   client_list->clients[client_index].username, buffer);

            char message_with_username[BUFFER_SIZE + USERNAME_SIZE + 8];
            snprintf(message_with_username,
                     sizeof(message_with_username), "[%s] : %s",
                     client_list->clients[client_index].username,
                     buffer);
            broadcast(client_list, message_with_username, client_index, strlen(message_with_username));
        }
    }

    pthread_mutex_lock(&client_list->mutex);
    close(client_list->clients[client_index].socket);
    for (int i = client_index; i < client_list->client_count - 1; ++i) {
        client_list->clients[i] = client_list->clients[i + 1];
    }
    client_list->client_count--;
    pthread_mutex_unlock(&client_list->mutex);
    pthread_exit(NULL);
}

void broadcast(ClientList *client_list, char *message,
               const int client_index, int size) {
    // Broadcast the message to all clients
    pthread_mutex_lock(&client_list->mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (i != client_index && client_list->clients[i].socket != -1) {
            // Send the message in the format "username: message"
            send(client_list->clients[i].socket,
                 message,
                 size, 0);
        }
    }
    pthread_mutex_unlock(&client_list->mutex);
}
// task 3
int main3(int argc, char *argv[]) {
    char buffer[1024];
    int named_pipe;

    int option;
    char *pipe_name = "myfifo";

    while ((option = getopt(argc, argv, "n:")) != -1) {
        switch (option) {
            case 'n':
                pipe_name = optarg;
                break;
            default:
                fprintf(stderr, "Использование: %s -n имя_канала\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (mkfifo(pipe_name, 0666) == -1) {
        perror("Ошибка при создании именованного канала");
        exit(EXIT_FAILURE);
    }

    ssize_t bytes_received = 0;

    while (1) {
        named_pipe = open(pipe_name, O_RDWR);
        bytes_received = read(named_pipe, buffer, sizeof(buffer));
        if (bytes_received <= 0) {
            close(named_pipe);
            break;
        }
        if (strcmp(buffer, "q") == 0) {
            close(named_pipe);
            break;
        }

        write(named_pipe, buffer, strlen(buffer));
        close(named_pipe);

        printf("Клиент: %s\n", buffer);
    }
    return 0;
}