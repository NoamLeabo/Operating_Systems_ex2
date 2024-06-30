#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include <string.h>
#include "copytree.h"

// print usage
void print_usage(const char *prog_name)
{
    fprintf(stderr, "Usage: %s [-l] [-p] <source_directory> <destination_directory>", prog_name);
    fprintf(stderr, "  -l: Copy symbolic links as links");
    fprintf(stderr, "  -p: Copy file permissions");
}

int main(int argc, char *argv[])
{
    int optioned = 1;
    int copy_symlinks = 0;
    int copy_permissions = 0;

    // HANDLE THE FLAGS HERE
    for (int i = 0; i < argc; i++)
    {
        if (!strcmp(argv[i], "-l"))
        {
            optioned++;
            copy_symlinks++;
            continue;
        }

        if (!strcmp(argv[i], "-p"))
        {
            optioned++;
            copy_permissions++;
        }
    }

    // we check validation of args
    if (optioned + 2 != argc)
    {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    // we save the src and dest parameters
    const char *src_dir = argv[optioned];
    const char *dest_dir = argv[optioned + 1];

    // w ecopy the dir
    copy_directory(src_dir, dest_dir, copy_symlinks, copy_permissions);

    return 0;
}
