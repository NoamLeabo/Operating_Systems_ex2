#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

const char *lock = "lockfile.lock";

void write_message(const char *message, int count)
{
    for (int i = 0; i < count; i++)
    {
        printf("%s\n", message);
        usleep((rand() % 100) * 1000); // Random delay between 0 and 99 milliseconds
    }
}

void locking_lock()
{
    while (open(lock, O_CREAT | O_EXCL, 0644) == -1)
    {
        usleep(1000); // Wait for 1ms before retrying to acquire the lock
    }
}

void unlocking_lock()
{
    if (remove(lock) != 0)
    {
        perror("Error deleting lock file");
        exit(1);
    }
}

int main(int argc, char const *argv[])
{
    if (argc < 4)
    {
        fprintf(stderr, "Usage: %s <message1> <message2> ... <count>", argv[0]);
        return 1;
    }

    int numOfForks = argc - 2;
    int numOfPrints = atoi(argv[argc - 1]);

    pid_t *children = (pid_t *)malloc(sizeof(pid_t) * numOfForks);

    for (int i = 0; i < numOfForks; i++)
    {
        pid_t newChild = fork();
        if (newChild == 0)
        {
            // child process
            locking_lock();
            write_message(argv[i + 1], numOfPrints);
            unlocking_lock();
            exit(0);
        }
        else if (newChild < 0)
        {
            perror("fork");
            exit(1);
        }
        else
            children[i] = newChild;
    }

    for (int i = 0; i < numOfForks; i++)
    {
        waitpid(children[i], NULL, 0);
    }

    free(children);

    return 0;
}
