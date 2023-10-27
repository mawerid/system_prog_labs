#include "../include/tasks.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// for task1
#define BUFFER_SIZE 10
int buffer[BUFFER_SIZE];
int count = 0;
pthread_cond_t full = PTHREAD_COND_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;

// for task2
pthread_cond_t can_write = PTHREAD_COND_INITIALIZER;
pthread_cond_t can_read = PTHREAD_COND_INITIALIZER;
int active_writers = 0;
int waiting_writers = 0;
int active_readers = 0;

// for task3
int thread1_counter = 0;
int thread2_counter = 0;
int thread3_counter = 0;
bool stop_threads = false;

void* producer(void* arg) {
    int item;
    for (int i = 0; i < 10; i++) {
        item = rand() % 100;
        pthread_mutex_lock(&mutex);
        while (count == BUFFER_SIZE)
            pthread_cond_wait(&empty, &mutex);
        buffer[count++] = item;
        printf("Producer produced item %d\n", item);
        pthread_cond_signal(&full);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void* consumer(void* arg) {
    int item;
    for (int i = 0; i < 10; i++) {
        pthread_mutex_lock(&mutex);
        while (count == 0)
            pthread_cond_wait(&full, &mutex);
        item = buffer[--count];
        printf("Consumer consumed item %d\n", item);
        pthread_cond_signal(&empty);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int task1(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Usage: %s 1 <num_producers> <num_consumers>\n", argv[0]);
        return 1;
    }

    int num_producers = atoi(argv[2]);
    int num_consumers = atoi(argv[3]);

    if (num_consumers > num_producers) {
        printf("Producers number must be at least equal consumers number.\n");
        return 1;
    }

    pthread_t producer_threads[num_producers];
    pthread_t consumer_threads[num_consumers];

    for (int i = 0; i < num_producers; i++)
        pthread_create(&producer_threads[i], NULL, producer, NULL);
    for (int i = 0; i < num_consumers; i++)
        pthread_create(&consumer_threads[i], NULL, consumer, NULL);

    for (int i = 0; i < num_producers; i++)
        pthread_join(producer_threads[i], NULL);
    for (int i = 0; i < num_consumers; i++)
        pthread_join(consumer_threads[i], NULL);

    return 0;
}

void* writer(void* arg) {
    char* filename = (char*)arg;
    usleep(rand() % 10000 + 5000);
    pthread_mutex_lock(&mutex);
    waiting_writers++;
    while (active_readers > 0 || active_writers > 0) {
        pthread_cond_wait(&can_write, &mutex);
    }
    waiting_writers--;
    active_writers++;
    pthread_mutex_unlock(&mutex);

    printf("Writer is writing to the file\n");
    FILE* file = fopen(filename, "a");
    if (file) {
        fprintf(file, "Data from writer\n");
        fclose(file);
    }

    pthread_mutex_lock(&mutex);
    active_writers--;
    if (waiting_writers > 0) {
        pthread_cond_signal(&can_write);
    } else {
        pthread_cond_broadcast(&can_read);
    }
    pthread_mutex_unlock(&mutex);

    return NULL;
}

void* reader(void* arg) {
    char* filename = (char*)arg;
    usleep(rand() % 10000 + 5000);
    pthread_mutex_lock(&mutex);
    while (active_writers > 0) {
        pthread_cond_wait(&can_read, &mutex);
    }
    active_readers++;
    pthread_mutex_unlock(&mutex);

    printf("Reader is reading from the file\n");
    FILE* file = fopen(filename, "r");
    if (file) {
        char buffer[256];
        while (fgets(buffer, sizeof(buffer), file) != NULL) {
            printf("Read: %s", buffer);
        }
        fclose(file);
    }

    pthread_mutex_lock(&mutex);
    active_readers--;
    if (active_readers == 0) {
        pthread_cond_signal(&can_write);
    }
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int task2(int argc, char* argv[]) {
    if (argc != 5) {
        printf("Usage: %s 2 <num_readers> <num_writers> <file_path>\n", argv[0]);
        return 1;
    }

    int num_readers = atoi(argv[2]);
    int num_writers = atoi(argv[3]);
    char* filename = argv[4];

    srand(time(NULL));

    pthread_t writer_threads[num_writers];
    pthread_t reader_threads[num_readers];

    for (int i = 0; i < num_writers; i++)
        pthread_create(&writer_threads[i], NULL, writer, filename);
    for (int i = 0; i < num_readers; i++)
        pthread_create(&reader_threads[i], NULL, reader, filename);

    for (int i = 0; i < num_writers; i++)
        pthread_join(writer_threads[i], NULL);
    for (int i = 0; i < num_readers; i++)
        pthread_join(reader_threads[i], NULL);

    return 0;
}

void* thread1(void* arg) {
    while (!stop_threads) {
        pthread_mutex_lock(&mutex);
        thread1_counter++;
        pthread_mutex_unlock(&mutex);
        usleep(10000);
    }
    return NULL;
}

void* thread2(void* arg) {
    while (!stop_threads) {
        pthread_mutex_lock(&mutex);
        thread2_counter++;
        pthread_mutex_unlock(&mutex);
        usleep(20000);
    }
    return NULL;
}

void* thread3(void* arg) {
    while (!stop_threads) {
        pthread_mutex_lock(&mutex);
        thread3_counter++;
        pthread_mutex_unlock(&mutex);
        usleep(30000);
    }
    return NULL;
}

void get_terminal_size(int* rows, int* cols) {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    *rows = w.ws_row;
    *cols = w.ws_col;
}

void move_cursor(int row, int col) {
    printf("\e[%d;%df", row, col);
}

int task3() {
    pthread_t t1, t2, t3;

    struct termios original_termios;
    tcgetattr(STDIN_FILENO, &original_termios);
    struct termios modified_termios = original_termios;
    modified_termios.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &modified_termios);

    int rows, cols;
    char is_first = 1;
    get_terminal_size(&rows, &cols);

    pthread_create(&t1, NULL, thread1, NULL);
    pthread_create(&t2, NULL, thread2, NULL);
    pthread_create(&t3, NULL, thread3, NULL);

    while (1) {
        if (stop_threads) {
            break;
        }

        int ch = getchar();
        if (ch == ' ') {
            if (is_first) {
                move_cursor(rows, 0);
                is_first = 0;
            } else
                move_cursor(rows - 4, 0);
            printf("Thread 1 counter: %d\n", thread1_counter);
            printf("Thread 2 counter: %d\n", thread2_counter);
            printf("Thread 3 counter: %d\n", thread3_counter);
            printf("Press 'q' to quit.\n");
        }
        if (ch == 'q') {
            stop_threads = true;
            pthread_join(t1, NULL);
            pthread_join(t2, NULL);
            pthread_join(t3, NULL);
            break;
        }
    }

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios);

    return 0;
}