#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include "buffered_open.h"

// Function to write to the buffered file
buffered_file_t *buffered_open(const char *pathname, int flags, ...)
{

    buffered_file_t *bf = (buffered_file_t *)malloc(sizeof(buffered_file_t));

    size_t fid = open(pathname, flags);
    if (fid < 0)
    {
        perror("open");
        exit(1);
    }

    bf->fd = fid;
    bf->read_buffer = (char *)malloc(sizeof(BUFFER_SIZE));
    bf->write_buffer = (char *)malloc(sizeof(BUFFER_SIZE));

    bf->read_buffer_size = 0;
    bf->write_buffer_size = 0;

    bf->write_buffer_pos = 0;
    bf->read_buffer_pos = 0;

    bf->preappend = 0;
    bf->flags = flags;

    return bf;
}

int buffered_flush(char *buffer)
{
    if (buffer == NULL)
    {
        return -1;
    }
    memset(buffer, 0, BUFFER_SIZE);
    return 0;
}

ssize_t push_buffer_content(buffered_file_t *bf, char *buf, size_t count)
{
    // we write the required count to the file
    int bytes_written = write(bf->fd, buf, count);

    // if not all the bytes were sent we notify and break
    if (bytes_written != count)
    {
        return -1;
    }
    // we reset the buffer status

    bf->write_buffer_pos = 0;
    if (buffered_flush(bf->write_buffer) < 0)
    {
        return -1;
    }
    return bytes_written;
}

ssize_t buffered_write(buffered_file_t *bf, const void *buf, size_t count)
{
    // casting the buf into a const
    char *b = (char *)buf;

    // trying to write each byte to the file_buffer
    for (int i = 0; i < count; i++)
    {
        // if there is no space left
        if (bf->write_buffer_pos == BUFFER_SIZE)
        {
            // we return the OS ptr to it's correct pos in the file using seek
            if (bf->read_buffer_pos != 0)
            {
                lseek(bf->fd, BUFFER_SIZE - bf->read_buffer_pos, SEEK_CUR);
                buffered_flush(bf->read_buffer);
                bf->read_buffer_pos = 0;
                bf->read_buffer_size = 0;
            }

            // we push what's in the buffer to the file
            if (push_buffer_content(bf, bf->write_buffer, BUFFER_SIZE) < 0)
            {
                return -1;
            }
        }

        bf->write_buffer[bf->write_buffer_pos++] = b[i];
    }
    return count;
}

ssize_t buffered_read(buffered_file_t *bf, void *buf, size_t count)
{
    // casting the buf into a const
    char *b = (char *)buf;

    // trying to read each byte from the file_buffer
    for (int i = 0; i < count; i++)
    {
        // if there is no more to read from the buffer
        if (bf->read_buffer_size == 0 || bf->read_buffer_pos == bf->read_buffer_size)
        {
            /* we will get to the buffer more content from the actual file */

            // first we push what's in write_buffer to the file
            push_buffer_content(bf, bf->write_buffer, bf->write_buffer_pos);
            bf->write_buffer_pos = 0;
            bf->write_buffer_size = 0;

            // we clear the read buffer
            if (buffered_flush(bf->read_buffer) < 0)
            {
                return -1;
            }

            ssize_t bytes_read = read(bf->fd, bf->read_buffer, BUFFER_SIZE);

            // case of error
            if (bytes_read < 0)
            {
                return -1;
            }

            // we reset the read_pos
            bf->read_buffer_pos = 0;
            // we set the bytes to the buffer
            bf->read_buffer_size = strlen(bf->read_buffer);
            // if we're at the end of the file
            if (bf->read_buffer_size == 0)
            {
                return i;
            }
        }

        b[i] = bf->read_buffer[bf->read_buffer_pos++];
    }
    return count;
}

int buffered_close(buffered_file_t *bf)
{
    /* we make sure we don't lose any written bytes */

    // we return the OS ptr to it's correct pos in the file using seek
    if (!bf->read_buffer_pos)
    {
        lseek(bf->fd, BUFFER_SIZE - bf->read_buffer_pos, SEEK_CUR);
        buffered_flush(bf->read_buffer);
        bf->read_buffer_pos = 0;
        bf->read_buffer_size = 0;
    }

    if (push_buffer_content(bf, bf->write_buffer, bf->write_buffer_pos) < 0)
    {
        return -1;
    }

    // we first free the buffers
    free(bf->read_buffer);
    free(bf->write_buffer);

    // we now close the file itself
    if (close(bf->fd) < -1)
    {
        return -1;
    }

    // we now free the entire buffered_file_t
    free(bf);
}

int main(int argc, char const *argv[])
{
    buffered_file_t *bf = buffered_open("tes.txt", O_RDWR);
    printf("the id is: %d\n", bf->fd);

    char re[5];
    if (buffered_read(bf, re, 2) == -1)
    {
        perror("buffered_write");
        return 1;
    }

    printf("%s\n",re);


    const char *text = "Hello, World!";
    if (buffered_write(bf, text, strlen(text)) == -1)
    {
        perror("buffered_write");
        return 1;
    }
    printf("%s\n", bf->write_buffer);

    if (buffered_read(bf, re, 2) == -1)
    {
        perror("buffered_write");
        return 1;
    }

    if (buffered_close(bf))
    {
        perror("buffered_close");
        return 1;
    }
    return 0;
}
