#include <getopt.h>

#include "../include/task1.h"
#include "../include/task2.h"

int main(int argc, char *argv[]) {
    int opt;
    char *content = NULL;
    char *filename = NULL;
    char *newname = NULL;
    char *dest = NULL;
    char *list_dir = NULL;

    if (argc > 1) {
        const char *short_options = "c::r:d:n::p::s:a:l:o:u:";
        const struct option long_options[] = {
            {"create", required_argument, NULL, 'c'},
            {"read", required_argument, NULL, 'r'},
            {"delete", required_argument, NULL, 'd'},
            {"rename", required_argument, NULL, 'n'},
            {"copy", required_argument, NULL, 'p'},
            {"size", required_argument, NULL, 's'},
            {"attributes", required_argument, NULL, 'a'},
            {"readonly", required_argument, NULL, 1},
            {"hidden", required_argument, NULL, 2},
            {"list", required_argument, NULL, 'l'},
            {"sort", required_argument, NULL, 'o'},
            {"count", required_argument, NULL, 'u'},
            {"removea", required_argument, NULL, 3},
            {"sortnum", required_argument, NULL, 4},
            {NULL, 0, NULL, 0}};

        while ((opt = getopt_long(argc, argv, short_options,
                                  long_options, NULL)) != -1) {
            switch (opt) {
                case 'c':
                    content = optarg;
                    if ((optind - 1) < argc && filename == NULL) {
                        filename = argv[optind - 1];
                    }
                    create_file(filename, content);
                    break;
                case 'r':
                    if ((optind - 1) < argc && filename == NULL) {
                        filename = argv[optind - 1];
                    }
                    read_file(filename);
                    break;
                case 'd':
                    if ((optind - 1) < argc && filename == NULL) {
                        filename = argv[optind - 1];
                    }
                    delete_file(filename);
                    break;
                case 'n':
                    newname = optarg;
                    if ((optind - 1) < argc && filename == NULL) {
                        filename = argv[optind - 1];
                    }
                    rename_file(filename, newname);
                    break;
                case 'p':
                    dest = optarg;
                    if ((optind - 1) < argc && filename == NULL) {
                        filename = argv[optind - 1];
                    }
                    copy_file(filename, dest);
                    break;
                case 's':
                    if ((optind - 1) < argc && filename == NULL) {
                        filename = argv[optind - 1];
                    }
                    display_file_size(filename);
                    break;
                case 'a':
                    if ((optind - 1) < argc && filename == NULL) {
                        filename = argv[optind - 1];
                    }
                    display_file_attributes(filename);
                    break;
                case 'l':
                    list_dir = optarg;
                    list_directory_contents(list_dir);
                    break;
                case 'o':
                    if ((optind - 1) < argc && filename == NULL) {
                        filename = argv[optind - 1];
                    }
                    sort_file_alphabetically(filename);
                    break;
                case 'u':
                    if ((optind - 1) < argc && filename == NULL) {
                        filename = argv[optind - 1];
                    }
                    count_letters(filename);
                    break;
                case 1:
                    if ((optind - 1) < argc && filename == NULL) {
                        filename = argv[optind - 1];
                    }
                    set_read_only(filename);
                    break;
                case 2:
                    if ((optind - 1) < argc && filename == NULL) {
                        filename = argv[optind - 1];
                    }
                    set_hidden(filename);
                    break;
                case 3:
                    if ((optind - 1) < argc && filename == NULL) {
                        filename = argv[optind - 1];
                    }
                    remove_and_count_a(filename);
                    break;
                case 4:
                    if ((optind - 1) < argc && filename == NULL) {
                        filename = argv[optind - 1];
                    }
                    sort_numeric_descending(filename);
                    break;
                default:
                    print_help();
                    return 1;
            }
        }
    } else {
        printf("Interactive mode. Enter 'help' for a list of commands.\n");

        char input[256];
        while (1) {
            printf("> ");
            if (fgets(input, sizeof(input), stdin) == NULL) {
                break;
            }

            // Удаляем символ новой строки
            input[strcspn(input, "\n")] = 0;

            if (strcmp(input, "help") == 0) {
                print_help();
            } else if (strcmp(input, "q") == 0 || strcmp(input, "quit") == 0) {
                printf("Exiting interactive mode.\n");
                break;
            } else if (strncmp(input, "create", 6) == 0) {
                // Пример ввода: create file.txt This is the content
                char *arg = input + 7;
                content = strchr(arg, ' ');
                if (content) {
                    *content = '\0';
                    content++;
                    create_file(arg, content);
                }
            } else if (strncmp(input, "read", 4) == 0) {
                // Пример ввода: read file.txt
                char *arg = input + 5;
                read_file(arg);
            } else if (strncmp(input, "delete", 6) == 0) {
                // Пример ввода: delete file.txt
                char *arg = input + 7;
                delete_file(arg);
            } else if (strncmp(input, "rename", 6) == 0) {
                // Пример ввода: rename file.txt newfile.txt
                char *arg = input + 7;
                newname = strchr(arg, ' ');
                if (newname) {
                    *newname = '\0';
                    newname++;
                    rename_file(arg, newname);
                }
            } else if (strncmp(input, "copy", 4) == 0) {
                // Пример ввода: copy source.txt destination.txt
                char *arg = input + 5;
                dest = strchr(arg, ' ');
                if (dest) {
                    *dest = '\0';
                    dest++;
                    copy_file(arg, dest);
                }
            } else if (strncmp(input, "size", 4) == 0) {
                // Пример ввода: size file.txt
                char *arg = input + 5;
                display_file_size(arg);
            } else if (strncmp(input, "attributes", 10) == 0) {
                // Пример ввода: attributes file.txt
                char *arg = input + 11;
                display_file_attributes(arg);
            } else if (strncmp(input, "read-only", 9) == 0) {
                // Пример ввода: read-only file.txt
                char *arg = input + 10;
                set_read_only(arg);
            } else if (strncmp(input, "hidden", 6) == 0) {
                // Пример ввода: hidden file.txt
                char *arg = input + 7;
                set_hidden(arg);
            } else if (strncmp(input, "list", 4) == 0) {
                // Пример ввода: list directory/
                char *arg = input + 5;
                list_directory_contents(arg);
            } else if (strncmp(input, "sort_alpha", 10) == 0) {
                // Пример ввода: sort_alpha file.txt
                char *arg = input + 11;
                sort_file_alphabetically(arg);
            } else if (strncmp(input, "count_letters", 13) == 0) {
                // Пример ввода: count_letters file.txt
                char *arg = input + 14;
                count_letters(arg);
            } else if (strncmp(input, "remove_a", 8) == 0) {
                // Пример ввода: remove_a file.txt
                char *arg = input + 9;
                remove_and_count_a(arg);
            } else if (strncmp(input, "sort_numeric", 12) == 0) {
                // Пример ввода: sort_numeric file.txt
                char *arg = input + 13;
                sort_numeric_descending(arg);
            } else {
                printf("Unknown command. Enter 'help' for command list.\n");
            }
        }
    }

    return 0;
}
