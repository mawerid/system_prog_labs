#include <dirent.h>
#include <openssl/sha.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/inotify.h>
#include <sys/time.h>
#include <unistd.h>

#define SHA256_DIGEST_LENGTH 32

// Structure to store file information
typedef struct {
  char path[PATH_MAX];
  unsigned char hash[SHA256_DIGEST_LENGTH];
} FileInfo;

// Global variables
FileInfo *fileInfos = NULL;
size_t numFiles = 0;

// File to store checksum list
const char *checksumFile = "checksums.csv";

// Function to calculate SHA-256 checksum
void calculate_sha256(const char *file_path, unsigned char *hash) {
  FILE *file = fopen(file_path, "rb");
  if (!file) {
    fprintf(stderr, "Error opening file: %s\n", file_path);
    exit(EXIT_FAILURE);
  }

  SHA256_CTX sha256;
  SHA256_Init(&sha256);

  const size_t buffer_size = 8192;
  unsigned char buffer[buffer_size];
  size_t bytesRead;

  while ((bytesRead = fread(buffer, 1, buffer_size, file)) != 0) {
    SHA256_Update(&sha256, buffer, bytesRead);
  }

  SHA256_Final(hash, &sha256);

  fclose(file);
}

// Function to update checksums for all files and save to file
void update_checksums() {
  FILE *checksumFilePtr = fopen(checksumFile, "w");
  if (!checksumFilePtr) {
    perror("Error opening checksum file");
    exit(EXIT_FAILURE);
  }

  for (size_t i = 0; i < numFiles; ++i) {
    calculate_sha256(fileInfos[i].path, fileInfos[i].hash);

    fprintf(checksumFilePtr, "%s,%s\n", fileInfos[i].path, fileInfos[i].hash);

    printf("%s: ", fileInfos[i].path);
    for (int j = 0; j < SHA256_DIGEST_LENGTH; ++j) {
      printf("%02x", fileInfos[i].hash[j]);
    }
    printf("\n");
  }

  fclose(checksumFilePtr);
}

// Function to handle file modification events
void handle_file_modification(int inotifyFd) {
  char buffer[4096];
  ssize_t bytesRead = read(inotifyFd, buffer, sizeof(buffer));

  if (bytesRead == -1) {
    perror("Error reading inotify events");
    exit(EXIT_FAILURE);
  }

  // Process inotify events
  struct inotify_event *event = (struct inotify_event *) buffer;
  while (event != NULL) {
    if (event->mask & IN_MODIFY) {
      printf("File modified: %s\n", event->name);

      // Find the modified file in the FileInfo array
      for (size_t i = 0; i < numFiles; ++i) {
        if (strcmp(event->name, basename(fileInfos[i].path)) == 0) {
          // Recalculate the checksum for the modified file
          calculate_sha256(fileInfos[i].path, fileInfos[i].hash);
          printf("Checksum updated for %s\n", fileInfos[i].path);
          break;
        }
      }
    }

    // Move to the next event in the buffer
    bytesRead -= sizeof(struct inotify_event) + event->len;
    if (bytesRead > 0) {
      event = (struct inotify_event *) ((char *) event
          + sizeof(struct inotify_event) + event->len);
    } else {
      event = NULL;
    }
  }

  // Update checksums and save to file after handling file modifications
  update_checksums();
}

// Function to handle timer signal
void handle_timer(int signum) {
  update_checksums();
  printf("Checksums updated.\n");
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <directory_path>\n", argv[0]);
    return EXIT_FAILURE;
  }

  const char *directory_path = argv[1];

  // Set up inotify for monitoring file modifications
  int inotifyFd = inotify_init();
  if (inotifyFd == -1) {
    perror("Error initializing inotify");
    return EXIT_FAILURE;
  }

  int wd = inotify_add_watch(inotifyFd, directory_path, IN_MODIFY);
  if (wd == -1) {
    perror("Error adding inotify watch");
    close(inotifyFd);
    return EXIT_FAILURE;
  }

  // Set up the initial checksums
  DIR *dir = opendir(directory_path);
  if (!dir) {
    fprintf(stderr, "Error opening directory: %s\n", directory_path);
    close(inotifyFd);
    return EXIT_FAILURE;
  }

  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    if (entry->d_type == DT_REG) {
      if (numFiles % 10 == 0) {
        fileInfos = realloc(fileInfos, (numFiles + 10) * sizeof(FileInfo));
      }

      snprintf(fileInfos[numFiles].path,
               PATH_MAX,
               "%s/%s",
               directory_path,
               entry->d_name);
      calculate_sha256(fileInfos[numFiles].path, fileInfos[numFiles].hash);
      numFiles++;
    }
  }

  closedir(dir);

  // Set up the timer for periodic checksum updates
  struct itimerval timer;
  timer.it_value.tv_sec = 30 * 60;  // 30 minutes
  timer.it_value.tv_usec = 0;
  timer.it_interval = timer.it_value;
  setitimer(ITIMER_REAL, &timer, NULL);

  // Set up signal handler for the timer
  signal(SIGALRM, handle_timer);

  // Main loop
  while (1) {
    // Use select to wait for either inotify events or timer expiration
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(inotifyFd, &rfds);
    struct timeval timeout;
    timeout.tv_sec = 1;  // Timeout of 1 second
    timeout.tv_usec = 0;

    int result = select(inotifyFd + 1, &rfds, NULL, NULL, &timeout);
    if (result == -1) {
      perror("Error in select");
      close(inotifyFd);
      return EXIT_FAILURE;
    } else if (result > 0) {
      // Inotify events
      if (FD_ISSET(inotifyFd, &rfds)) {
        handle_file_modification(inotifyFd);
      }
    } else {
      // Timer expired
      handle_timer(SIGALRM);
    }
  }

  // Cleanup
  inotify_rm_watch(inotifyFd, wd);
  close(inotifyFd);
  free(fileInfos);

  return EXIT_SUCCESS;
}
