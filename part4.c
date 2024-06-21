#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include <string.h>

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

int buffered_clean(char *buffer)
{
    if (buffer == NULL)
    {
        return -1;
    }
    memset(buffer, 0, strlen(buffer));
    return 0;
}

void copy_file(const char *src, const char *dest, int copy_symlinks, int copy_permissions)
{
    // we check if the src is a linke
    struct stat st;
    if (lstat(src, &st) < 0)
    {
        perror("lstat");
        exit(1);
    }

    // if src is a link and we're required to copy this as a symlink
    if (copy_symlinks && S_ISLNK(st.st_mode))
    {
        // we check to which file in the dir we should bw linking to
        char buffer[4096];
        int linkNum = readlink(src, buffer,4096);
        // we create the symlink
        if (symlink(buffer, dest) < 0)
        {
            perror("symlink");
            exit(1);
        } 
        return;
    }

    // if we need to cpy the src content regardless
    else
    {
        // we open both the src and the dest
        int destFid = open(dest, O_CREAT | O_WRONLY);
        int srcFid = open(src, O_RDWR);

        // if we failed we exit
        if (destFid < 0 || srcFid < 0)
        {
            perror("open");
            exit(1);
        }

        // to know how much bytes we pass
        int total_bytes_written_to_dest = 0;
        int total_bytes_read_from_src = 0;

        // a buffer we are gonna transfer the data with
        char transfer[4096];

        // we read from the file
        int bytes_read = read(srcFid, transfer, 4096);
        // updating the counter
        total_bytes_read_from_src += bytes_read;

        // as long as we still read from the file
        while (bytes_read > 0)
        {
            // we write to the BU
            int bytes_written = write(destFid, transfer, bytes_read);
            // update the counter
            total_bytes_written_to_dest += bytes_written;
            // clear the buffer
            buffered_clean(transfer);
            // we read the next chunk
            bytes_read = read(srcFid, transfer, 4096);
            // updating the counter
            total_bytes_read_from_src += bytes_read;
        }

        // if we have got an error we exit
        if (bytes_read < 0)
        {
            perror("read");
            exit(1);
        }

        // if we lost info
        if (total_bytes_read_from_src != total_bytes_written_to_dest)
        {
            perror("information lost");
            exit(1);
        }

        // we close the files
        close(srcFid);
        close(destFid);
    }

    // we remove the write permit
    if (chmod(dest, 0) < 0)
    {
        perror("chmod");
        exit(1);
    }

    // if we need to cpy the permissions
    if (copy_permissions)
    {
        // we first get the src permissions
        struct stat *file = (struct stat *)malloc(sizeof(struct stat));
        if (!file)
        {
            perror("malloc");
            exit(1);
        }
        int tryStat = stat(src, file);
        int flags = file->st_mode;
        // we free the struct
        free(file);

        // we set the permissions
        if (chmod(dest, flags) < 0)
        {
            perror("chmod");
            exit(1);
        }
    }
}

void copy_directory(const char *src, const char *dest, int copy_symlinks, int copy_permissions)
{
    // we create dest dir
    if (mkdir(dest, 0755) < 0)
    {
        return;
    }

    // we open the src dir
    DIR *dir = opendir(src);
    if (!dir)
    {
        perror("opendir");
        exit(1);
    }

    // start going through the entries
    struct dirent *openDir;
    while ((openDir = readdir(dir)) != NULL)
    {
        // entry name
        char *name = openDir->d_name;
        // slashed name
        char *slashed = concatenate("/", name);
        // new paths
        char *newSrcPath = concatenate(src, slashed);
        char *newDestPath = concatenate(dest, slashed);

        // IF DIR TYPE
        if (openDir->d_type == 4)
        {
            if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
            {
                continue;
            }
            // we copy the dir
            copy_directory(newSrcPath, newDestPath, copy_symlinks, copy_permissions);
        }

        // IF FILE TYPE
        else if (openDir->d_type == 8)
        {
            // we copy the file
            copy_file(newSrcPath, newDestPath, copy_symlinks, copy_permissions);  
        }

        // IF LINK TYPE
        else if (openDir->d_type == 10)
        {
            // we copy the file
            copy_file(newSrcPath, newDestPath, copy_symlinks, copy_permissions);  
        }

        // free the new strings
        free(slashed);
        free(newSrcPath);
        free(newDestPath);
    }
    return;
}

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
    if (optioned + 2 != argc) {
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