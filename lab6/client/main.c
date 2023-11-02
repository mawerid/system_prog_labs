#include <arpa/inet.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

// task 1
int main(int argc, char *argv[]) {
    int client_socket;
    struct sockaddr_in server_addr;
    char message[1024];

    int option;
    char *ip = NULL;
    int port = 12345;

    while ((option = getopt(argc, argv, "i:p:")) != -1) {
        switch (option) {
            case 'i':
                ip = optarg;
                break;
            case 'p':
                port = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Использование: %s -i IP-адрес -p порт\n",
                        argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (ip == NULL) {
        fprintf(stderr, "Необходимо указать IP-адрес сервера.\n");
        exit(EXIT_FAILURE);
    }

    // Создаем соксет
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Ошибка при создании соксета");
        exit(EXIT_FAILURE);
    }

    // Настройка адреса сервера
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    // Подключаемся к серверу
    if (connect(client_socket, (struct sockaddr *)&server_addr,
                sizeof(server_addr)) == -1) {
        perror("Ошибка при подключении к серверу");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    while (1) {
        // Вводим сообщение
        printf("Введите сообщение: ");
        fgets(message, sizeof(message), stdin);

        // Отправляем сообщение серверу
        send(client_socket, message, strlen(message), 0);

        // Получаем "эхосообщение" от сервера
        recv(client_socket, message, sizeof(message), 0);
        printf("Сервер: %s\n", message);
    }

    // Закрываем соксет
    close(client_socket);

    return 0;
}

// task 2
int main(int argc, char *argv[]) {
    int client_socket;
    struct sockaddr_in server_addr;
    char message[1024];

    int option;
    char *ip = NULL;
    int port = 12345;

    while ((option = getopt(argc, argv, "i:p:")) != -1) {
        switch (option) {
            case 'i':
                ip = optarg;
                break;
            case 'p':
                port = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Использование: %s -i IP-адрес -p порт\n",
                        argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (ip == NULL) {
        fprintf(stderr, "Необходимо указать IP-адрес сервера.\n");
        exit(EXIT_FAILURE);
    }

    // Создаем соксет
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Ошибка при создании соксета");
        exit(EXIT_FAILURE);
    }

    // Настройка адреса сервера
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    // Подключаемся к серверу
    if (connect(client_socket, (struct sockaddr *)&server_addr,
                sizeof(server_addr)) == -1) {
        perror("Ошибка при подключении к серверу");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    while (1) {
        // Вводим имя файла
        printf("Введите имя файла: ");
        fgets(message, sizeof(message), stdin);
        message[strcspn(message, "\n")] = '\0';  // Убираем символ новой строки

        // Отправляем имя файла серверу
        send(client_socket, message, strlen(message), 0);

        // Открываем файл для чтения
        FILE *file = fopen(message, "rb");
        if (file == NULL) {
            perror("Ошибка при открытии файла");
            continue;
        }

        // Отправляем файл
        size_t bytes_read;
        while ((bytes_read = fread(message, 1, sizeof(message), file) > 0)) {
            send(client_socket, message, bytes_read, 0);
        }

        fclose(file);
        printf("Файл отправлен.\n");
    }

    // Закрываем соксет
    close(client_socket);

    return 0;
}

// task 3
int main(int argc, char *argv[]) {
    char message[1024];
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

    // Открываем именованный канал для записи
    named_pipe = open(pipe_name, O_WRONLY);

    while (1) {
        // Вводим сообщение
        printf("Введите сообщение: ");
        fgets(message, sizeof(message), stdin);
        message[strcspn(message, "\n")] = '\0';  // Убираем символ новой строки

        // Отправляем сообщение в канал
        write(named_pipe, message, strlen(message));

        // Читаем "эхосообщение" из канала
        int echo_pipe = open(pipe_name, O_RDONLY);
        read(echo_pipe, message, sizeof(message));
        printf("Сервер: %s\n");
        close(echo_pipe);
    }

    // Закрываем именованный канал
    close(named_pipe);

    return 0;
}
