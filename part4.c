#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>  
#include <dirent.h>

void copy_file(const char *src, const char *dest, int copy_symlinks, int copy_permissions)
{
    if (copy_symlinks)
    {
        if(symlink(src, dest) < 0){
            perror("symlink");
            exit(1);
        }
    }
    else
        if(link(src, dest) < 0){
            perror("link");
            exit(1);
        }

    if (copy_permissions)
    {
        struct stat *file = (struct stat *) malloc (sizeof(struct stat));
        if (!file)
        {
            perror("malloc");
            exit(1);
        }
        
        int tryStat = stat(src, file);
        int flags = file->st_mode;
        free(file);
        if (chmod(dest, flags) < 0)
        {
            perror("chmod");
            exit(1);
        }
    }
}

void cpy_dir_rec(const char *src, const char *dest){
    
    if (mkdir(dest, 0) < 0){
        return;
    }

    DIR* dir = opendir(src);
    if (!dir)
    {
        perror("opendir");
        exit(1);
    }

    struct dirent* openDir;
    while (openDir = readdir(dir) != NULL)
    {
        openDir->d_type
    }
    
    
    

}

void copy_directory(const char *src, const char *dest, int copy_symlinks, int copy_permissions) {}

int main(int argc, char const *argv[])
{
    copy_file("tocpy.txt", "cpy.txt", 0, 0);
    return 0;
}
