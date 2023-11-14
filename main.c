#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define BMP_HEADER_WIDTH_OFFSET 18
/// #define BMP_

typedef struct {
    int size;
} BMPFileStats;

static int readInt(int fd)
{
    /// TO BE IMPLEMENTED
    return 0;
}

static BMPFileStats* getBMPFileStats(int fd)
{
    /// Check file descriptor
    if (fd < 0)
    {
        perror("Error reading input file");
        return NULL;
    }
    /// Move cursor to read width & height
    if (lseek(fd, BMP_HEADER_WIDTH_OFFSET, SEEK_SET) < 0)
    {
        perror("Error parsing file");
        return NULL;
    }

    int width = 0;
    int ret_val = read(fd, &width, sizeof(width));
    if (ret_val < 0)
    {
        perror("Error reading file");
        return NULL;
    }
    if (ret_val != sizeof(width))
    {
        fprintf(stderr, "Invalid bitmap file header.\n");
        return NULL;
    }

    struct stat st;
    if (fstat(fd, &st) < 0)
    {
        perror("Error reading stats");
        return NULL;
    }

    return NULL;
}

static int printBMPFileStats()
{
    return 0;
}

int main(int argc, char **argv)
{
    /// Check args
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    fprintf(stdout, "Given file path : %s\n", argv[1]);

    /// Open file
    int fd = open(argv[1], O_RDONLY);
    getBMPFileStats(fd);

    return 0;
}
