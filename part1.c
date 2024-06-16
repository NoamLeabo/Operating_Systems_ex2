#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>

char* concatenate(const char* str1, const char* str2) {
    char* result = malloc(strlen(str1) + strlen(str2) + 1); // +1 for the null-terminator
    if (result == NULL) {
        perror("malloc");
        exit(1);
    }
    strcpy(result, str1);
    strcat(result, str2);
    return result;
}

int main(int argc, char const *argv[])
{
    if (argc != 5)
    {
        fprintf(stderr, "Usage: %s <parent_message> <child1_message> <child2_message> <count>", argv[0]);
        return 1;
    }

    if (fork() == 0)
    {
        // first child
        int statusFC;
        if (fork() == 0)
        {
            // second child
            int file = open("output.txt", O_CREAT | O_WRONLY | O_APPEND, 0777);
            for (int i = 0; i < atoi(argv[4]); i++)
            {
                printf("writing to file: %s\n", argv[2]);
                char* buffer = concatenate(argv[2], "\n");
                int wr = write(file, buffer, strlen(buffer));
                free(buffer);
            }
            close(file);
        }
        else
        {
            // first child
            wait(&statusFC);
            int file = open("output.txt", O_CREAT | O_WRONLY | O_APPEND, 0777);
            for (int i = 0; i < atoi(argv[4]); i++)
            {
                printf("writing to file: %s\n", argv[3]);
                char* buffer = concatenate(argv[3], "\n");
                int wr = write(file, buffer, strlen(buffer));
                free(buffer);
            }
            close(file);
        }
    }
    else
    {
        // parent
        int statusP;
        wait(&statusP);
        int file = open("output.txt", O_CREAT | O_WRONLY | O_APPEND, 0777);
        for (int i = 0; i < atoi(argv[4]); i++)
        {
            printf("writing to file: %s\n", argv[1]);
            char* buffer = concatenate(argv[1], "\n");
            int wr = write(file, buffer, strlen(buffer));
            free(buffer);
        }
        close(file);
    }
    return 0;
}
