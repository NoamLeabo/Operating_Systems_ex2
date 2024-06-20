#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdarg.h>
#include "buffered_open.h"

// Function to write to the buffered file
buffered_file_t *buffered_open(const char *pathname, int flags, ...)
{

    buffered_file_t *bf = (buffered_file_t *)malloc(sizeof(buffered_file_t));
    if (!bf)
    {
        perror("malloc");
        exit(1);
    }
    

    // checking if other args were provided
    va_list ptr;
    va_start(ptr, flags);

    int extra = va_arg(ptr, int);
    va_end(ptr);

    // check for the preappend flag
    if (flags - O_PREAPPEND >= 0)
    {
        bf->flags = flags - O_PREAPPEND;
        bf->preappend = 1;
    }
    else
    {
        bf->preappend = 0;
        bf->flags = flags;
    }

    size_t fid;

    // open the File and hold it in the struct
    if (extra > 0)
    {
        fid = open(pathname, bf->flags, extra);
    }
    else
        fid = open(pathname, bf->flags);

    // in case of an error
    if (fid < 0)
    {
        perror("open");
        exit(1);
    }

    // we save also the FIle's fid
    bf->fd = fid;

    // we create the buffers
    bf->read_buffer = (char *)malloc(sizeof(BUFFER_SIZE));
    bf->write_buffer = (char *)malloc(sizeof(BUFFER_SIZE));

    // in case of an error
    if (!bf->read_buffer || !bf->write_buffer)
    {
        perror("malloc");
        exit(1);
    }
    
    // we set the sizes
    bf->read_buffer_size = 0;
    bf->write_buffer_size = 0;

    // we set the positions
    bf->write_buffer_pos = 0;
    bf->read_buffer_pos = 0;

    return bf;
}

// a Func to clean a given buffer
int buffered_flush(char *buffer)
{
    if (buffer == NULL)
    {
        return -1;
    }
    memset(buffer, 0, strlen(buffer));
    return 0;
}

// a Func to write an intire content in a buffer to the file
ssize_t push_buffer_content(buffered_file_t *bf, char *buf, size_t count)
{
    // in case the preappend flag ain't turned on
    if (!bf->preappend)
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
        // we flush the buffer
        if (buffered_flush(bf->write_buffer) < 0)
        {
            return -1;
        }
        // return the num of written bytes
        return bytes_written;
    }
    // if preappend flag is turned on
    else
    {
        // we save the old offset so we could put it back at the end
        off_t offset = lseek(bf->fd, 0, SEEK_CUR);
        // we add the num of bytes we will be writting
        offset += count;

        // we open a file to save the data
        int backUp = open("temp.txt", O_CREAT | O_RDWR, 0666);
        // to know how much we shloud bring back
        int total_bytes_written_to_backUp = 0;
        // a buffer we are gonna transfer the data with
        char transfer[BUFFER_SIZE];

        // we read from the file
        int bytes_read = read(bf->fd, transfer, BUFFER_SIZE);
        // as long as we still read from the file
        while (bytes_read > 0)
        {
            // we write to the BU
            int bytes_written = write(backUp, transfer, bytes_read);
            // update the counter
            total_bytes_written_to_backUp += bytes_written;
            // clear the buffer
            buffered_flush(transfer);
            // we read the next chunk
            bytes_read = read(bf->fd, transfer, BUFFER_SIZE);
        }
        // if we have got an error we exit
        if (bytes_read < 0)
        {
            return -1;
        }

        // we reset the ptr bach to their places
        lseek(bf->fd, -total_bytes_written_to_backUp, SEEK_CUR);
        lseek(backUp, -total_bytes_written_to_backUp, SEEK_CUR);

        // we now write the required data the the file overriding the content
        int bytes_written = write(bf->fd, buf, count);

        // if not all the bytes were sent we notify and break
        if (bytes_written != count)
        {
            return -1;
        }

        // we reset the buffer status
        bf->write_buffer_pos = 0;
        // we clear the write_buffer
        if (buffered_flush(bf->write_buffer) < 0)
        {
            return -1;
        }

        // start writting back the BU
        int bytes_returned_to_OG_file = 0;
        bytes_read = read(backUp, transfer, BUFFER_SIZE);
        while (bytes_read > 0)
        {
            // we write the data back to OG
            int bytes_written = write(bf->fd, transfer, bytes_read);
            // update the counter
            bytes_returned_to_OG_file += bytes_written;
            // clearing the buffer
            buffered_flush(transfer);
            // read again
            bytes_read = read(backUp, transfer, BUFFER_SIZE);
        }
        // in case of an error
        if (bytes_read < 0)
        {
            return -1;
        }

        // in case we lost date we fail
        if (total_bytes_written_to_backUp != bytes_returned_to_OG_file)
        {
            return -1;
        }

        // we close the file
        close(backUp);
        remove("temp.txt");

        // we put back the old offset + the byte we have written
        lseek(bf->fd, offset, SEEK_SET);
        return 0;
    }
}

// write to the write buffer
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
                lseek(bf->fd, -(bf->read_buffer_size - bf->read_buffer_pos), SEEK_CUR);
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

        // we set the content to the buffer
        bf->write_buffer[bf->write_buffer_pos++] = b[i];
    }
    //return the num of bytes written
    return count;
}

// read from the read buffer 
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
            if (bf->write_buffer_pos != 0)
            {
                push_buffer_content(bf, bf->write_buffer, bf->write_buffer_pos);
                bf->write_buffer_pos = 0;
                bf->write_buffer_size = 0;
            }

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
        // read the content from the buffer
        b[i] = bf->read_buffer[bf->read_buffer_pos++];
    }
    // return the bytes that were read
    return count;
}

// a func that closes a buffers_file
int buffered_close(buffered_file_t *bf)
{
    /* we make sure we don't lose any written bytes */

    // we return the OS ptr to it's correct pos in the file using seek
    if (!bf->read_buffer_pos)
    {
        lseek(bf->fd, -(BUFFER_SIZE - bf->read_buffer_pos), SEEK_CUR);
        buffered_flush(bf->read_buffer);
        bf->read_buffer_pos = 0;
        bf->read_buffer_size = 0;
    }

    if (bf->write_buffer_pos != bf->write_buffer_size)
    {
        if (push_buffer_content(bf, bf->write_buffer, bf->write_buffer_pos) < 0)
        {
            return -1;
        }
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
    buffered_file_t *bf = buffered_open("tes.txt", O_CREAT | O_RDWR);
    printf("the id is: %d\n", bf->fd);

    char FIR[5];
    if (buffered_read(bf, FIR, 0) == -1)
    {
        perror("buffered_write");
        return 1;
    }

    printf("%s\n", FIR);

    const char *text = "HELLOWORLD";
    if (buffered_write(bf, text, strlen(text)) == -1)
    {
        perror("buffered_write");
        return 1;
    }

    printf("%s\n", bf->write_buffer);

    char SEC[5];
    if (buffered_read(bf, SEC, 2) == -1)
    {
        perror("buffered_write");
        return 1;
    }

    printf("%s\n", SEC);

    if (buffered_close(bf))
    {
        perror("buffered_close");
        return 1;
    }
    return 0;
}
