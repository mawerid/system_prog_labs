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
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char buffer[1024];

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

    // Создаем сокет
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Ошибка при создании соксета");
        exit(EXIT_FAILURE);
    }

    // Настройка адреса сервера
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    // Привязываем соксет к адресу и порту
    if (bind(server_socket, (struct sockaddr *)&server_addr,
             sizeof(server_addr)) == -1) {
        perror("Ошибка при привязке соксета");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Слушаем соксет на подключения
    if (listen(server_socket, 5) == -1) {
        perror("Ошибка при прослушивании соксета");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Принимаем соединения
    client_socket = accept(server_socket, (struct sockaddr *)&client_addr,
                           &client_addr_len);
    if (client_socket == -1) {
        perror("Ошибка при принятии соединения");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    while (1) {
        // Читаем сообщение от клиента
        recv(client_socket, buffer, sizeof(buffer), 0);
        printf("Клиент: %s\n", buffer);

        // Отправляем "эхосообщение" клиенту
        send(client_socket, buffer, strlen(buffer), 0);
    }

    // Закрываем соксеты
    close(client_socket);
    close(server_socket);

    return 0;
}

// task 2
int main(int argc, char *argv[]) {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char buffer[1024];

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
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Ошибка при создании соксета");
        exit(EXIT_FAILURE);
    }

    // Настройка адреса сервера
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    // Привязываем соксет к адресу и порту
    if (bind(server_socket, (struct sockaddr *)&server_addr,
             sizeof(server_addr)) == -1) {
        perror("Ошибка при привязке соксета");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Слушаем соксет на подключения
    if (listen(server_socket, 5) == -1) {
        perror("Ошибка при прослушивании соксета");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Принимаем соединения
    client_socket = accept(server_socket, (struct sockaddr *)&client_addr,
                           &client_addr_len);
    if (client_socket == -1) {
        perror("Ошибка при принятии соединения");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    while (1) {
        // Принимаем имя файла от клиента
        recv(client_socket, buffer, sizeof(buffer), 0);
        printf("Клиент: %s\n", buffer);

        // Открываем файл для записи
        FILE *file = fopen(buffer, "wb");
        if (file == NULL) {
            perror("Ошибка при открытии файла");
            continue;
        }

        // Принимаем и записываем файл
        size_t bytes_received;
        while ((bytes_received = recv(client_socket, buffer,
                                      sizeof(buffer), 0) > 0)) {
            fwrite(buffer, 1, bytes_received, file);
        }

        fclose(file);
        printf("Файл получен и сохранен.\n");
    }

    // Закрываем соксеты
    close(client_socket);
    close(server_socket);

    return 0;
}

// task 3
int main(int argc, char *argv[]) {
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

    // Создаем именованный канал
    if (mkfifo(pipe_name, 0666) == -1) {
        perror("Ошибка при создании именованного канала");
        exit(EXIT_FAILURE);
    }

    // Открываем именованный канал для чтения
    named_pipe = open(pipe_name, O_RDONLY);

    while (1) {
        // Читаем сообщение из канала
        read(named_pipe, buffer, sizeof(buffer));
        printf("Клиент: %s\n", buffer);

        // Отправляем "эхосообщение" обратно в канал
        int echo_pipe = open(pipe_name, O_WRONLY);
        write(echo_pipe, buffer, strlen(buffer));
        close(echo_pipe);
    }

    // Закрываем именованный канал
    close(named_pipe);

    return 0;
}