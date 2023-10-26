#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define PI 3.14159

// variant 6

void func_1();
void func_2();
void func_3(int n);
void func_4(int n);
void insert_sort(int* mass, size_t size);
void circe_area(double* area, double radius);

enum Gender { MALE,
              FEMALE };

typedef struct DateBirth {
    unsigned day;
    unsigned month;
    unsigned year;
} DateBirth;

typedef struct Student {
    char* surname;
    DateBirth birthday;
    enum Gender gender;
} Student;

const char* exampleSurnames[] = {
    "Smith", "Johnson", "Brown", "Davis", "Wilson",
    "Taylor", "Anderson", "Martinez", "Jackson", "Moore",
    "Harris", "Clark", "Young", "White", "Walker",
    "Allen", "King", "Green", "Scott", "Adams",
    "Baker", "Gonzalez", "Nelson", "Carter", "Hill"};

typedef struct Book {
    char* author;
    char* title;
    char* publisher;
    unsigned year;
    double cost;
} Book;

const char* exampleAuthors[] = {
    "Author1", "Author2", "Author3", "Author4", "Author5",
    "Author6", "Author7", "Author8", "Author9", "Author10"};

const char* exampleTitles[] = {
    "Title1", "Title2", "Title3", "Title4", "Title5",
    "Title6", "Title7", "Title8", "Title9", "Title10"};

const char* examplePublishers[] = {
    "Publisher with k1", "Publisher2", "Publisher with k3", "Publisher4", "Publisher with k5",
    "Publisher6", "Publisher7", "Publisher with k8", "Publisher9", "Publisher with k10"};

void initializeBook(Book* book);
void deinitializeBook(Book* book);
size_t selectBooks(Book* src, Book* dest, size_t size);
void sortBooks(Book* books, size_t size);
void printBook(Book* book);
void copyBook(Book* dest, Book* src);

int main() {
    // func_1();
    // func_2();

    // int n;
    // printf("Enter the n:");
    // scanf("%d", &n);

    // func_3(n);
    // func_4(n);

    // double rad = 3.7;
    // double* area;
    // area = (double*)malloc(sizeof(double));
    // if (area == NULL) {
    //     printf("Allocation error.\n");
    //     return 1;
    // }
    // circe_area(area, rad);
    // printf("Area = %lf\n", *area);
    // rad = 5.2;
    // circe_area(area, rad);
    // printf("Area = %lf\n", *area);
    // free(area);

    // int numStudents = 25;
    // Student* students = (Student*)malloc(numStudents * sizeof(Student));
    // if (students == NULL) {
    //     printf("Allocation error.\n");
    //     return 1;
    // }

    // srand(time(NULL));

    // for (int i = 0; i < numStudents; i++) {
    //     students[i].surname = (char*)malloc(
    //         strlen(exampleSurnames[i]) * sizeof(char));
    //     if (students[i].surname == NULL) {
    //         printf("Allocation error.\n");
    //         for (int j = 0; j < i; j++)
    //             free(students[j].surname);
    //         free(students);
    //         return 1;
    //     }
    //     strcpy(students[i].surname, exampleSurnames[i]);
    //     students[i].birthday.day = rand() % 28 + 1;
    //     students[i].birthday.month = rand() % 12 + 1;
    //     students[i].birthday.year = 1995 + rand() % 10;
    //     students[i].gender = rand() % 2 == 0 ? MALE : FEMALE;
    // }

    // printf("Female students born in December:\n");
    // for (int i = 0; i < numStudents; i++) {
    //     if (students[i].gender == FEMALE && students[i].birthday.month == 12) {
    //         printf("%s\n", students[i].surname);
    //     }
    // }

    // for (int i = 0; i < numStudents; i++) {
    //     free(students[i].surname);
    // }
    // free(students);

    int numBooks = 10;
    Book* library = (Book*)malloc(numBooks * sizeof(Book));

    if (library == NULL) {
        printf("Allocation error.\n");
        return 1;
    }

    for (int i = 0; i < numBooks; i++) {
        initializeBook(&library[i]);

        library[i].author = (char*)malloc(
            (strlen(exampleAuthors[i]) + 1) * sizeof(char));
        if (library[i].author == NULL) {
            printf("Allocation error.\n");
            for (int j = 0; j < i; j++)
                deinitializeBook(&library[j]);
            free(library);
            return 1;
        }
        strcpy(library[i].author, exampleAuthors[i]);
        library[i].author[strlen(exampleAuthors[i])] = '\0';

        library[i].title = (char*)malloc(
            (strlen(exampleTitles[i]) + 1) * sizeof(char));
        if (library[i].title == NULL) {
            printf("Allocation error.\n");
            for (int j = 0; j < i; j++)
                deinitializeBook(&library[j]);
            free(library[i].author);
            free(library);
            return 1;
        }
        strcpy(library[i].title, exampleTitles[i]);
        library[i].title[strlen(exampleTitles[i])] = '\0';

        library[i].publisher = (char*)malloc(
            (strlen(examplePublishers[i]) + 1) * sizeof(char));
        if (library[i].publisher == NULL) {
            printf("Allocation error.\n");
            for (int j = 0; j < i; j++)
                deinitializeBook(&library[j]);
            free(library[i].author);
            free(library[i].title);
            free(library);
            return 1;
        }
        strcpy(library[i].publisher, examplePublishers[i]);
        library[i].publisher[strlen(examplePublishers[i])] = '\0';
        library[i].year = 2000 + i;
        library[i].cost = 10.99 + i;
    }

    Book* selectedBooks = NULL;
    int selectedSize = selectBooks(library, selectedBooks, numBooks);

    if (selectedSize == 0) {
        printf("No books with 'k' in the publisher's name found.\n");
    } else {
        sortBooks(selectedBooks, selectedSize);
        printf("Selected books sorted by title:\n");
        for (int i = 0; i < selectedSize; i++) {
            printBook(&selectedBooks[i]);
        }
    }

    for (int i = 0; i < selectedSize; i++) {
        deinitializeBook(&selectedBooks[i]);
    }

    for (int i = 0; i < numBooks; i++) {
        deinitializeBook(&library[i]);
    }

    free(library);
    free(selectedBooks);

    return 0;
}

void func_1() {
    int a, b;
    printf("Enter a, b:");
    scanf("%d %d", &a, &b);

    int* ptr_a;
    int* ptr_b;
    ptr_a = &a;
    ptr_b = &b;

    if (*ptr_a > *ptr_b) {
        (*ptr_a) *= 5;
        (*ptr_b) -= 5;
    } else {
        (*ptr_b) *= 5;
        (*ptr_a) -= 5;
    }

    printf("a: %d\nb: %d\n", *ptr_a, *ptr_b);
}

void func_2() {
    float* ptr_1;
    float* ptr_2;
    float* ptr_3;

    ptr_1 = (float*)malloc(sizeof(float));
    if (ptr_1 == NULL) {
        printf("Allocation error.\n");
        return;
    }
    ptr_2 = (float*)malloc(sizeof(float));
    if (ptr_2 == NULL) {
        printf("Allocation error.\n");
        free(ptr_1);
        return;
    }
    ptr_3 = (float*)malloc(sizeof(float));
    if (ptr_3 == NULL) {
        printf("Allocation error.\n");
        free(ptr_1);
        free(ptr_2);
        return;
    }

    *ptr_1 = 1.1;
    *ptr_2 = 1.2;
    *ptr_3 = 1.3;

    printf("%lf %lf %lf\n", *ptr_1, *ptr_2, *ptr_3);

    *ptr_1 /= 2;

    printf("%lf %lf %lf\n", *ptr_1, *ptr_2, *ptr_3);

    free(ptr_1);
    free(ptr_2);
    free(ptr_3);
}

void func_3(int n) {
    int* p;
    int* q;

    p = (int*)calloc(n, sizeof(int));
    if (p == NULL) {
        printf("Allocation error.\n");
        return;
    }
    q = (int*)calloc(n, sizeof(int));
    if (q == NULL) {
        printf("Allocation error.\n");
        free(p);
        return;
    }

    srand(time(NULL));
    int size = n / 2;

    for (int i = 0; i < n; i++) {
        p[i] = rand() % n - size;
        q[i] = rand() % n - size;
        printf("p[%d] = %d  |   q[%d] = %d\n", i, p[i], i, q[i]);
    }

    int count_p = 0;
    int count_q = 0;
    for (int i = 0; i < n; i++) {
        if (p[i] > 0)
            count_p++;
        if (q[i] > 0)
            count_q++;
    }

    if (count_p > count_q)
        printf("P: %d\n", count_p);
    else if (count_q > count_p)
        printf("Q: %d\n", count_q);
    else
        printf("Equal\n");

    free(p);
    free(q);
}

void func_4(int n) {
    int* a;

    a = (int*)calloc(n, sizeof(int));
    if (a == NULL) {
        printf("Allocation error.\n");
        return;
    }

    srand(time(NULL));
    int size = 2;

    for (int i = 0; i < n; i++) {
        a[i] = rand() % n - size;
        printf("a[%d] = %d\n", i, a[i]);
    }

    int* b_old;
    int* b;
    int count = 0;
    b_old = (int*)calloc(n, sizeof(int));
    if (b_old == NULL) {
        printf("Allocation error.\n");
        free(a);
        return;
    }

    for (int i = 0; i < n; i++) {
        if (a[i] > 0 && a[i] % 3 == 0) {
            b_old[count] = a[i];
            printf("b[%d] = %d\n", count, b_old[count]);
            count++;
        }
    }
    b = (int*)realloc(b_old, count * sizeof(int));
    if (b == NULL) {
        printf("Allocation error.\n");
        free(b_old);
        free(a);
        return;
    }

    insert_sort(b, count);

    for (int i = 0; i < count; i++) {
        printf("b[%d] = %d\n", i, b[i]);
    }

    free(b_old);
    free(a);
    free(b);
}

void insert_sort(int* mass, size_t size) {
    int min_index;
    int tmp;

    for (size_t i = 0; i < size; i++) {
        min_index = i;
        for (size_t j = i; j < size; j++) {
            if (mass[j] < mass[min_index])
                min_index = j;
        }

        tmp = mass[i];
        mass[i] = mass[min_index];
        mass[min_index] = tmp;
    }
}

void circe_area(double* area, double radius) {
    if (area == NULL)
        return;
    *area = PI * radius * radius;
}

void initializeBook(Book* book) {
    book->author = NULL;
    book->title = NULL;
    book->publisher = NULL;
    book->year = 0;
    book->cost = 0.0;
}

void deinitializeBook(Book* book) {
    free(book->author);
    free(book->title);
    free(book->publisher);
}

size_t selectBooks(Book* src, Book* dest, size_t size) {
    size_t destSize = 0;
    size_t current = 0;
    Book* new_dest;
    for (size_t i = 0; i < size; i++) {
        if (strchr(src[i].publisher, 'k') != NULL) {
            if (destSize == current) {
                current += 1;
                new_dest = (Book*)malloc(current * sizeof(Book));
                for (int j = 0; j < current - 1; j++) {
                    copyBook(&new_dest[j], &dest[j]);
                }
                for (int j = 0; j < current - 1; j++) {
                    deinitializeBook(&dest[j]);
                }
                free(dest);
                dest = new_dest;
            }
            copyBook(&dest[destSize], &src[i]);
            destSize++;
        }
    }
    return destSize;
}

void sortBooks(Book* books, size_t size) {
    Book* tmp;
    tmp = (Book*)malloc(sizeof(Book));
    initializeBook(tmp);
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (strcmp(books[j].title, books[j + 1].title) > 0) {
                memcpy(tmp, &books[j], sizeof(Book));
                memcpy(&books[j], &books[j + 1], sizeof(Book));
                memcpy(&books[j + 1], tmp, sizeof(Book));
            }
        }
    }
    // deinitializeBook(tmp);
    free(tmp);
}

void printBook(Book* book) {
    printf("Author: %s\n", book->author);
    printf("Title: %s\n", book->title);
    printf("Publisher: %s\n", book->publisher);
    printf("Year: %d\n", book->year);
    printf("Cost: %.2f\n", book->cost);
    printf("-----------\n");
}

void copyBook(Book* dest, Book* src) {
    if (src == NULL) {
        return;
    }
    if (dest == NULL) {
        dest = (Book*)malloc(sizeof(Book));
    }

    initializeBook(dest);

    dest->author = (char*)malloc(
        (strlen(src->author) + 1) * sizeof(char));
    if (dest->author == NULL) {
        printf("Allocation error.\n");
        free(dest);
        return;
    }
    strcpy(dest->author, src->author);
    dest->author[strlen(src->author)] = '\0';

    dest->title = (char*)malloc(
        (strlen(src->title) + 1) * sizeof(char));
    if (dest->title == NULL) {
        printf("Allocation error.\n");
        free(dest->author);
        free(dest);
        return;
    }
    strcpy(dest->title, src->title);
    dest->title[strlen(src->title)] = '\0';

    dest->publisher = (char*)malloc(
        (strlen(src->publisher) + 1) * sizeof(char));
    if (dest->publisher == NULL) {
        printf("Allocation error.\n");
        free(dest->author);
        free(dest->title);
        free(dest);
        return;
    }
    strcpy(dest->publisher, src->publisher);
    dest->publisher[strlen(src->publisher)] = '\0';
    dest->year = src->year;
    dest->cost = src->cost;
}
