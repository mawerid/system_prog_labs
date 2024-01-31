#include <arpa/inet.h>
#include <fcntl.h>
#include <getopt.h>
#include <libgen.h>
#include <netinet/in.h>
#include <poll.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define SERVER_BUFFER_SIZE 1024
#define USERNAME_SIZE 32

typedef struct {
    int socket;
    char username[USERNAME_SIZE];
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int exit_flag;
} ClientInfo;

void *receive_messages(void *arg);
void *send_messages(void *arg);
void send_file(int socket, const char *filename);
void receive_file(int socket, const char *filename);

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <ip_address> <port> <username>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *ip_address = argv[1];
    int port = atoi(argv[2]);
    const char *username = argv[3];

    if (strlen(username) >= USERNAME_SIZE) {
        fprintf(stderr,
                "Username is too long (max %d characters)\n",
                USERNAME_SIZE - 1);
        exit(EXIT_FAILURE);
    }

    // Create socket
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(ip_address);
    server_address.sin_port = htons(port);

    if (connect(client_socket,
                (struct sockaddr *) &server_address,
                sizeof(server_address)) == -1) {
        perror("Error connecting to server");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    printf("Connected to server at %s:%d\n", ip_address, port);

    // Send the username to the server
    send(client_socket, username, strlen(username), 0);

    // Initialize synchronization objects
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

    // Prepare client information to pass to threads
    ClientInfo client_info;
    client_info.socket = client_socket;
    strncpy(client_info.username, username, sizeof(client_info.username));
    client_info.mutex = mutex;
    client_info.cond = cond;
    client_info.exit_flag = 0;

    // Create threads for receiving and sending messages
    pthread_t receive_thread, send_thread;
    if (pthread_create(&receive_thread,
                       NULL,
                       receive_messages,
                       (void *) &client_info) != 0 ||
        pthread_create(&send_thread, NULL, send_messages, (void *) &client_info)
            != 0) {
        perror("Error creating threads");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    // Wait for threads to finish
    pthread_join(receive_thread, NULL);
    pthread_join(send_thread, NULL);

    // Cleanup synchronization objects
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    // Close the client socket
    close(client_socket);

    return 0;
}

void *receive_messages(void *arg) {
    ClientInfo *client_info = (ClientInfo *) arg;
    char buffer[SERVER_BUFFER_SIZE];
    int bytes_received;

    while (1) {
        if (client_info->exit_flag)
            pthread_exit(NULL);

        // Receive data from the server
        bytes_received = recv(client_info->socket, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
            printf("Server disconnected\n");
            break;
        }

        // Null-terminate the received data
        buffer[bytes_received] = '\0';
        printf("%s\n", buffer);

        // Display server response
        if (strncmp(buffer, "/file", 5) == 0) {
            printf("HI\n");
            char filename[SERVER_BUFFER_SIZE];
            sscanf(buffer, "/file %s", filename);

            printf("%s\n", filename);
            printf("%s\n", basename(filename));

            // Receive the file from the server
            receive_file(client_info->socket, basename(filename));
        } else {
            // Display server response
            printf("\n\033[F%s\n", buffer);
        }
    }

    // Signal the main thread to exit
    pthread_mutex_lock(&client_info->mutex);
    client_info->exit_flag = 1;
    pthread_cond_signal(&client_info->cond);
    pthread_mutex_unlock(&client_info->mutex);

    pthread_exit(NULL);
}

void *send_messages(void *arg) {
    ClientInfo *client_info = (ClientInfo *) arg;
    char buffer[SERVER_BUFFER_SIZE];
    struct pollfd pfd;
    pfd.fd = fileno(stdin);
    pfd.events = POLLIN;

    while (1) {
        if (client_info->exit_flag)
            pthread_exit(NULL);

        printf("\033[E[%s] : ", client_info->username);
        // if (poll(&pfd, 1, 0) > 0 && pfd.revents) {
        // if (feof(stdin)) {
        // fflush(stdout);
        // Get user input
        fgets(buffer, sizeof(buffer), stdin);

        // Remove newline character from the input
        buffer[strcspn(buffer, "\n")] = '\0';

        // Send the user input to the server
        send(client_info->socket, buffer, strlen(buffer), 0);

        // Check if the user wants to exit
        if (strcmp(buffer, "/exit") == 0) {
            printf("Exiting...\n");
            break;
        } else if (strncmp(buffer, "/file", 5) == 0) {
            char filename[SERVER_BUFFER_SIZE];
            sscanf(buffer, "/file %s", filename);

            // Send the file to the server
            send_file(client_info->socket, filename);
        }
        // }
    }

    // Signal the main thread to exit
    pthread_mutex_lock(&client_info->mutex);
    client_info->exit_flag = 1;
    pthread_cond_signal(&client_info->cond);
    pthread_mutex_unlock(&client_info->mutex);

    pthread_exit(NULL);
}

void send_file(int socket, const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Error opening file");
        send(socket, "/endfile", strlen("/endfile"), 0);
        return;
    }

    fseek(file, 0L, SEEK_END);
    int size = ftell(file);
    rewind(file);

    char *buffer = malloc(size * sizeof(char));
    size_t bytes_read;

    send(socket, &size, sizeof(int), 0);

    // Send the file content to the server
    if (bytes_read = fread(buffer,
                           size,
                           1, file) > 0) {
        send(socket, buffer, size, 0);
    }

    fclose(file);
    free(buffer);

    printf("File '%s' sent to server\n", filename);
}

void receive_file(int socket, const char *filename) {
    int size = 0;
    recv(socket, &size, sizeof(int), 0);

    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        perror("Error opening file for writing");
        return;
    }

    char *buffer = malloc(size * sizeof(char));
    int bytes_received;
    int length = 0;

    while (length < size) {
        // Receive data from the server
        bytes_received = recv(socket, buffer + length, size - length + 1, 0);
        printf("Received  %d \n", bytes_received);
        if (bytes_received <= 0) {
            printf("Server disconnected\n");
            fclose(file);
            free(buffer);
            return;
        }

        length += bytes_received;

        // Write the received data to the file
    }
    printf("Writing...  %d\n", size);

    fwrite(buffer, size, 1, file);

    printf("File '%s' received from server\n", filename);

    fclose(file);
    free(buffer);
}

// task 3
int main3(int argc, char *argv[]) {
    char message[1024];
    int named_pipe;

    int option;
    char *pipe_name = "myfifo";

    while ((option = getopt(argc, argv, "n:")) != -1) {
        switch (option) {
            case 'n':pipe_name = optarg;
                break;
            default:
                fprintf(stderr,
                        "Использование: %s -n имя_канала\n",
                        argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    named_pipe = open(pipe_name, O_RDWR);

    while (1) {
        printf("Введите сообщение: ");
        fgets(message, sizeof(message), stdin);
        message[strcspn(message, "\n")] = '\0';  // Убираем символ новой строки
        if (strcmp(message, "q") == 0) {
            write(named_pipe, message, strlen(message));
            break;
        }

        write(named_pipe, message, strlen(message));
        message[0] = 'A';
        read(named_pipe, message, sizeof(message));
        printf("Сервер: %s\n", message);
    }

    close(named_pipe);

    return 0;
}
