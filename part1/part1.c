#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>

// a function to concatenate two strings
char *concatenate(const char *str1, const char *str2)
{
    // +1 for the null-terminator
    char *result = malloc(strlen(str1) + strlen(str2) + 1);
    // result will be null if malloc fails we indicate so
    if (result == NULL)
    {
        perror("malloc");
        exit(1);
    }

    // copy str1 to result
    strcpy(result, str1);
    // concatenate str2 to result
    strcat(result, str2);
    // return the result
    return result;
}

int main(int argc, char const *argv[])
{
    // check if the number of arguments is correct
    if (argc != 5)
    {
        // print the usage of the program
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
            // wecreate the file
            int file = open("output.txt", O_CREAT | O_WRONLY | O_APPEND, 0777);
            // check if the file is created successfully
            if (file == -1)
            {
                // print an error message
                perror("open");
                return 1;
            }
            // write the message to the file
            for (int i = 0; i < atoi(argv[4]); i++)
            {
                // concatenate the message with a newline character
                char *buffer = concatenate(argv[2], "\n");
                // write the message to the file
                int wr = write(file, buffer, strlen(buffer));
                // check if the write is successful
                if (wr == -1)
                {
                    // print an error message
                    perror("write");
                    return 1;
                }
                // free the buffer
                free(buffer);
            }
            // close the file
            close(file);
        }
        // check if the fork failed
        else if (getpid() == -1)
        {
            // print an error message
            perror("fork");
            exit(1);
        }
        else
        {
            // first child
            // wait for the second child to finish
            wait(&statusFC);
            // open the file
            int file = open("output.txt", O_CREAT | O_WRONLY | O_APPEND, 0777);
            // check if the file is opened successfully
            if (file == -1)
            {
                // print an error message
                perror("open");
                return 1;
            }
            // write the message to the file
            for (int i = 0; i < atoi(argv[4]); i++)
            {
                // concatenate the message with a newline character
                char *buffer = concatenate(argv[3], "\n");
                // write the message to the file
                int wr = write(file, buffer, strlen(buffer));
                // check if the write is successful
                if (wr == -1)
                {
                    // print an error message
                    perror("write");
                    return 1;
                }
                // free the buffer
                free(buffer);
            }
            // close the file
            close(file);
        }
    }
    // check if the fork failed
    else if (getpid() == -1)
    {
        // print an error message
        perror("fork");
        exit(1);
    }
    else
    {
        // parent
        int statusP;
        // wait for the first child to finish
        wait(&statusP);
        // open the file
        int file = open("output.txt", O_CREAT | O_WRONLY | O_APPEND, 0777);
        // check if the file is opened successfully
        if (file == -1)
        {
            // print an error message
            perror("open");
            return 1;
        }
        // write the message to the file
        for (int i = 0; i < atoi(argv[4]); i++)
        {
            int wr;
            if (i == atoi(argv[4]) - 1) {
              // write the message to the file withouy \n
              wr = write(file, argv[1], strlen(argv[1]));
            } else {
            // concatenate the message with a newline character
            char *buffer = concatenate(argv[1], "\n");
            // write the message to the file
            wr = write(file, buffer, strlen(buffer));
            // free the buffer
            free(buffer);
            }
            // check if the write is successful
            if (wr == -1)
            {
                // print an error message
                perror("write");
                return 1;
            }
        }
        // close the file
        close(file);
    }

    return 0;
}
