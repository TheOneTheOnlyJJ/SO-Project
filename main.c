#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define BMP_HEADER_WIDTH_OFFSET 18
// #define BMP_

static int read_int(int fd)
{
    // TO BE IMPLEMENTED
}

int main(int argc, char **argv)
{
    // Check args
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    fprintf(stdout, "Given file path : %s\n", argv[1]);
    
    // Open file
    int fd = open(argv[1], O_RDONLY);
    if (fd < 0)
    {
        perror("Error reading input file");
        exit(EXIT_FAILURE);
    }
    
    // Move cursor to read width & height
    if (lseek(fd, BMP_HEADER_WIDTH_OFFSET, SEEK_SET) < 0)
    {
        perror("Error parsing file");
        exit(EXIT_FAILURE);
    }
    
    int ret_val = 0;
    int width = 0;
    if (ret_val = read(fd, &width, sizeof(width)) < 0)
    {
        perror("Error reading file");
        exit(EXIT_FAILURE);
    }
    if (ret_val != sizeof(width))
    {
        fprintf(stderr, "Invalid bitmap file header.\n");
        exit(EXIT_FAILURE);
    }
    
    struct stat st;
    if (fstat(fd, &st) < 0)
    {
        perror("Error reading stats");
        exit(EXIT_FAILURE);
    }
    
    return 0;
}
