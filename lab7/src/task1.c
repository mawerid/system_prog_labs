#include "../include/task1.h"

void print_help() {
    printf("Usage: file_util [options] [filename]\n");
    printf("Options:\n");
    printf("  -c, --create <content>   Create a file and write content to it\n");
    printf("  -r, --read              Read and display the file content\n");
    printf("  -d, --delete            Delete the file\n");
    printf("  -n, --rename <newname>  Rename the file\n");
    printf("  -p, --copy <dest>       Copy the file to the destination\n");
    printf("  -s, --size              Display the file size\n");
    printf("  -a, --attributes        Display file attributes\n");
    printf("  --readonly              Set file read-only attribute\n");
    printf("  --hidden                Set file hidden attribute\n");
    printf("  -l, --list <dir>        List contents of a directory\n");
    printf("  -o, --sort              Sort file content alphabetically\n");
    printf("  -u, --count             Count uppercase and lowercase letters\n");
    printf("  --removea              Remove all 'a' characters and append count\n");
    printf("  --sortnum              Sort numeric file content in descending order\n");
}

void create_file(const char *filename, const char *content) {
    FILE *file = fopen(filename, "w");
    if (file) {
        fputs(content, file);
        fclose(file);
        printf("File created and content written successfully.\n");
    } else {
        perror("Error creating file");
    }
}

void read_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file) {
        char buffer[256];
        while (fgets(buffer, sizeof(buffer), file)) {
            printf("%s", buffer);
        }
        fclose(file);
        printf("\n");
    } else {
        perror("Error reading file");
    }
}

void delete_file(const char *filename) {
    if (remove(filename) == 0) {
        printf("File deleted successfully.\n");
    } else {
        perror("Error deleting file");
    }
}

void rename_file(const char *filename, const char *newname) {
    if (rename(filename, newname) == 0) {
        printf("File renamed successfully.\n");
    } else {
        perror("Error renaming file");
    }
}

void copy_file(const char *src, const char *dest) {
    FILE *source = fopen(src, "rb");
    FILE *destination = fopen(dest, "wb");
    if (source && destination) {
        char buffer[4096];
        size_t n;
        while ((n = fread(buffer, 1, sizeof(buffer), source)) > 0) {
            fwrite(buffer, 1, n, destination);
        }
        fclose(source);
        fclose(destination);
        printf("File copied successfully.\n");
    } else {
        perror("Error copying file");
    }
}

void display_file_size(const char *filename) {
    struct stat st;
    if (stat(filename, &st) == 0) {
        printf("File size: %ld bytes\n", st.st_size);
    } else {
        perror("Error getting file size");
    }
}

void display_file_attributes(const char *filename) {
    struct stat st;
    if (stat(filename, &st) == 0) {
        printf("File attributes:\n");
        printf("  Size: %ld bytes\n", st.st_size);
        printf("  Permissions: %o\n", st.st_mode & 0777);
        printf("  Hidden: %s\n", (st.st_mode & S_IFMT) == S_IFDIR ? "No" : "Yes");
        printf("  Read-only: %s\n", (st.st_mode & S_IWUSR) ? "No" : "Yes");
    } else {
        perror("Error getting file attributes");
    }
}

void set_read_only(const char *filename) {
    if (chmod(filename, S_IRUSR) == 0) {
        printf("File set as read-only.\n");
    } else {
        perror("Error setting read-only attribute");
    }
}

void set_hidden(const char *filename) {
    if (chmod(filename, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH) == 0) {
        printf("File set as hidden.\n");
    } else {
        perror("Error setting hidden attribute");
    }
}

void list_directory_contents_recursive(const char *dirname) {
    printf("Contents of directory '%s':\n", dirname);
    DIR *dir = opendir(dirname);
    if (dir) {
        struct dirent *entry;
        while ((entry = readdir(dir))) {
            if (entry->d_type == DT_DIR) {
                if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                    char subdir[256];
                    snprintf(subdir, sizeof(subdir), "%s/%s", dirname, entry->d_name);
                    list_directory_contents_recursive(subdir);
                }
            } else {
                printf("%s\n", entry->d_name);
            }
        }
        closedir(dir);
    } else {
        perror("Error listing directory contents");
    }
}

void list_directory_contents(const char *dirname) {
    list_directory_contents_recursive(dirname);
}

// void list_directory_contents(const char *dirname) {
//     printf("Contents of directory '%s':\n", dirname);
//     DIR *dir = opendir(dirname);
//     if (dir) {
//         struct dirent *entry;
//         while ((entry = readdir(dir))) {
//             printf("%s\n", entry->d_name);
//         }
//         closedir(dir);
//     } else {
//         perror("Error listing directory contents");
//     }
// }