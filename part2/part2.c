#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

// the file that will be used as a lock
const char *lock = "lockfile.lock";

// a function to write a message to the standard output
void write_message(const char *message, int count)
{
    for (int i = 0; i < count; i++)
    {
        printf("%s\n", message);
        usleep((rand() % 100) * 1000); // Random delay between 0 and 99 milliseconds
    }
}

// a function to lock the lock file
void locking_lock()
{
    /*
    try to open the lock file with the O_CREAT and O_EXCL flags which will
    create the file if it doesn't exist otherwise it will fail
    */

    while (open(lock, O_CREAT | O_EXCL, 0644) == -1)
    {
        usleep(1000); // Wait for 1ms before retrying to acquire the lock
    }
}

// a function to unlock the lock file
void unlocking_lock()
{
    // remove the lock file
    if (remove(lock) != 0)
    {
        // print an error message if the file is not removed successfully
        perror("Error deleting lock file");
        exit(1);
    }
}

int main(int argc, char const *argv[])
{
    // check if the number of arguments is correct
    if (argc < 4)
    {
        fprintf(stderr, "Usage: %s <message1> <message2> ... <count>", argv[0]);
        return 1;
    }

    // the number of forks is the number of arguments - 2
    int numOfForks = argc - 2;
    // the number of prints is the last argument
    int numOfPrints = atoi(argv[argc - 1]);

    // an array to store the children's pids
    pid_t *children = (pid_t *)malloc(sizeof(pid_t) * numOfForks);

    // check if malloc is successful
    if (children == NULL)
    {
        perror("malloc");
        exit(1);
    }

    // create the children
    for (int i = 0; i < numOfForks; i++)
    {
        // create a new child
        pid_t newChild = fork();
        if (newChild == 0)
        {
            // child process

            // locking the lock and preventing other children from writing
            locking_lock();
            // write the message
            write_message(argv[i + 1], numOfPrints);
            // release the lock
            unlocking_lock();
            // exit the child
            exit(0);
        }
        // check if the fork is successful
        else if (newChild < 0)
        {
            // print an error message
            perror("fork");
            exit(1);
        }
        else
            // parent process

            // store the child's pid
            children[i] = newChild;
    }

    // parent will wait for all children to finish
    for (int i = 0; i < numOfForks; i++)
    {
        waitpid(children[i], NULL, 0);
    }

    // free the children array
    free(children);

    return 0;
}
