#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include "buffered_open.h"
 
// Function to write to the buffered file
buffered_file_t *buffered_open(const char *pathname, int flags, ...){

    buffered_file_t * bf = (buffered_file_t *) malloc (sizeof(buffered_file_t));
    
    size_t fid = open(pathname, 0);
    if (fid < 0) {
        perror("open");
        exit(1);
    }

    bf->fd = fid;
    bf->read_buffer = malloc (sizeof(BUFFER_SIZE));
    bf->write_buffer = malloc (sizeof(BUFFER_SIZE)); 

    bf->read_buffer_size = BUFFER_SIZE;
    bf->write_buffer_size = BUFFER_SIZE;
    
    bf->write_buffer_pos = 0;
    bf->read_buffer_pos = 0;

    bf->preappend = 0;
    bf->flags = flags;

    return bf;
}

int main(int argc, char const *argv[])
{
    buffered_file_t * bf = buffered_open("tes.txt", 0);
    printf("the id is: %d\n", bf->fd);
    return 0;
}
