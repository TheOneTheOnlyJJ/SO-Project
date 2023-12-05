#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define RASTER_DATA_OFFSET 10

typedef struct {
    char numeFisier[BUFSIZ], drepturiUser[BUFSIZ], drepturiGrup[BUFSIZ], drepturiAltii[BUFSIZ];
    int dimensiune, inaltime, lungime, userId, nrLegaturi;
    time_t timpUltimaModificare;
} ReqStats;

static void getFileStats(int in, char* filename, ReqStats* reqStats)
{
    struct stat inStat;
    int buffer;
    /// Copy filename
    strcpy(reqStats->numeFisier, filename);
    /// Dimensiune
    lseek(in, 18, SEEK_SET);
    /// Lungime si inaltime doar pentru fisiere de tip bmp
    if (strstr(filename, ".bmp"))
    {
        /// Lungime
        read(in, &buffer, 4);
        reqStats->lungime = buffer;
        /// Inaltime
        read(in, &buffer, 4);
        reqStats->inaltime = buffer;
    }
    else
    {
        reqStats->lungime = -1;
        reqStats->inaltime = -1;
    }
    /// Get other stats with fstat
    fstat(in, &inStat);
    reqStats->dimensiune = inStat.st_size;
    reqStats->nrLegaturi = inStat.st_nlink;
    reqStats->userId = inStat.st_uid;
    reqStats->timpUltimaModificare = inStat.st_mtime;
    /// Is dir
    strcpy(reqStats->drepturiUser, S_ISDIR(inStat.st_mode) ? "d" : "-");
    /// User rights
    strcat(reqStats->drepturiUser, (inStat.st_mode & S_IRUSR) ? "r" : "-");
    strcat(reqStats->drepturiUser, (inStat.st_mode & S_IWUSR) ? "w" : "-");
    strcat(reqStats->drepturiUser, (inStat.st_mode & S_IXUSR) ? "x" : "-");
    /// Group rights
    strcat(reqStats->drepturiGrup, (inStat.st_mode & S_IRGRP) ? "r" : "-");
    strcat(reqStats->drepturiGrup, (inStat.st_mode & S_IWGRP) ? "w" : "-");
    strcat(reqStats->drepturiGrup, (inStat.st_mode & S_IXGRP) ? "x" : "-");
    /// Others rights
    strcat(reqStats->drepturiAltii, (inStat.st_mode & S_IROTH) ? "r" : "-");
    strcat(reqStats->drepturiAltii, (inStat.st_mode & S_IWOTH) ? "w" : "-");
    strcat(reqStats->drepturiAltii, (inStat.st_mode & S_IXOTH) ? "x" : "-");

    return;
}

static void printFileStats(int out, ReqStats stats)
{
    char buffer[BUFSIZ];
    /// Nume fisier
    sprintf(buffer, "Nume fisier: %s.\n", stats.numeFisier);
    write(out, buffer, strlen(buffer));
    strcpy(buffer, "");
    /// Dimensiune
    sprintf(buffer, "Dimensiune: %d.\n", stats.dimensiune);
    write(out, buffer, strlen(buffer));
    strcpy(buffer, "");
    /// Lungime si inaltime doar pentru fisiere de tip bmp
    if (stats.inaltime != -1 && stats.lungime != -1)
    {
        /// Lungime
        sprintf(buffer, "Lungime: %d.\n", stats.lungime);
        write(out, buffer, strlen(buffer));
        strcpy(buffer, "");
        /// Inaltime
        sprintf(buffer, "Inaltime: %d.\n", stats.inaltime);
        write(out, buffer, strlen(buffer));
        strcpy(buffer, "");
    }
    /// User ID
    sprintf(buffer, "User ID: %d.\n", stats.userId);
    write(out, buffer, strlen(buffer));
    strcpy(buffer, "");
    /// Numar legaturi
    sprintf(buffer, "Numar legaturi: %d.\n", stats.nrLegaturi);
    write(out, buffer, strlen(buffer));
    strcpy(buffer, "");
    /// Last modified time
    sprintf(buffer, "Timpul ultimei modificari: %li.\n", stats.timpUltimaModificare);
    write(out, buffer, strlen(buffer));
    strcpy(buffer, "");
    /// Drepturi de acces user
    sprintf(buffer, "Drepturi de acces user: %s.\n", stats.drepturiUser);
    write(out, buffer, strlen(buffer));
    strcpy(buffer, "");
    /// Drepturi de acces grup
    sprintf(buffer, "Drepturi de acces grup: %s.\n", stats.drepturiGrup);
    write(out, buffer, strlen(buffer));
    strcpy(buffer, "");
    /// Drepturi de acces altii
    sprintf(buffer, "Drepturi de acces altii: %s.\n", stats.drepturiAltii);
    write(out, buffer, strlen(buffer));
    strcpy(buffer, "");

    return;
}

static void printDirStats(int out, struct stat* filestat, char* dirname)
{
    char buffer[BUFSIZ];
    /// Nume director
    sprintf(buffer, "Nume director: %s.\n", dirname);
    write(out, buffer, strlen(buffer));
    strcpy(buffer, "");
    /// User ID
    sprintf(buffer, "User ID: %d.\n", filestat->st_uid);
    write(out, buffer, strlen(buffer));
    strcpy(buffer, "");
    /// Drepturi de acces user
    sprintf(
        buffer,
        "Drepturi de acces user: %s%s%s.\n",
        (filestat->st_mode & S_IRUSR) ? "r" : "-",
        (filestat->st_mode & S_IWUSR) ? "w" : "-",
        (filestat->st_mode & S_IXUSR) ? "x" : "-"
    );
    write(out, buffer, strlen(buffer));
    strcpy(buffer, "");
    /// Drepturi de acces grup
    sprintf(
        buffer,
        "Drepturi de acces grup: %s%s%s.\n",
        (filestat->st_mode & S_IRGRP) ? "r" : "-",
        (filestat->st_mode & S_IWGRP) ? "w" : "-",
        (filestat->st_mode & S_IXGRP) ? "x" : "-"
    );
    write(out, buffer, strlen(buffer));
    strcpy(buffer, "");
    /// Drepturi de acces altii
    sprintf(
        buffer,
        "Drepturi de acces altii: %s%s%s.\n",
        (filestat->st_mode & S_IROTH) ? "r" : "-",
        (filestat->st_mode & S_IWOTH) ? "w" : "-",
        (filestat->st_mode & S_IXOTH) ? "x" : "-"
    );
    write(out, buffer, strlen(buffer));
    strcpy(buffer, "");

    return;
}

static void printSymLinkStats(int out, struct stat* filestat, char* dirname, ReqStats reqstats, long targetFileDimension)
{
    char buffer[BUFSIZ];
    /// Nume link simbolic
    sprintf(buffer, "Nume link simbolic: %s.\n", dirname);
    write(out, buffer, strlen(buffer));
    strcpy(buffer, "");
    /// Dimensiune self
    sprintf(buffer, "Dimensiune fisier: %ld.\n", filestat->st_size);
    write(out, buffer, strlen(buffer));
    strcpy(buffer, "");
    /// Dimensiune target
    sprintf(buffer, "Dimensiune fisier target: %ld.\n", targetFileDimension);
    write(out, buffer, strlen(buffer));
    strcpy(buffer, "");
    /// Drepturi de acces user
    sprintf(
        buffer,
        "Drepturi de acces user: %s%s%s.\n",
        (filestat->st_mode & S_IRUSR) ? "r" : "-",
        (filestat->st_mode & S_IWUSR) ? "w" : "-",
        (filestat->st_mode & S_IXUSR) ? "x" : "-"
    );
    write(out, buffer, strlen(buffer));
    strcpy(buffer, "");
    /// Drepturi de acces grup
    sprintf(
        buffer,
        "Drepturi de acces grup: %s%s%s.\n",
        (filestat->st_mode & S_IRGRP) ? "r" : "-",
        (filestat->st_mode & S_IWGRP) ? "w" : "-",
        (filestat->st_mode & S_IXGRP) ? "x" : "-"
    );
    write(out, buffer, strlen(buffer));
    strcpy(buffer, "");
    /// Drepturi de acces altii
    sprintf(
        buffer,
        "Drepturi de acces altii: %s%s%s.\n",
        (filestat->st_mode & S_IROTH) ? "r" : "-",
        (filestat->st_mode & S_IWOTH) ? "w" : "-",
        (filestat->st_mode & S_IXOTH) ? "x" : "-"
    );
    write(out, buffer, strlen(buffer));
    strcpy(buffer, "");

    return;
}

static void assembleDirEntFilePath(char* entName, char* outRelPath, char *outDir)
{
    char fileName[BUFSIZ];
    /// Handle current directory
    if (strcmp(entName, ".") == 0)
    {
        strcpy(fileName, "current_directory_statistics.txt");
    }
    /// Handle previous directory
    else if (strcmp(entName, "..") == 0)
    {
        strcpy(fileName, "father_directory_statistics.txt");
    }
    /// Handle content directories
    else
    {
        /// Handle file extensions gracefully
        char temp[BUFSIZ];
        strcpy(temp, entName);
        char* token = strtok(temp, ".");
        strcpy(fileName, token);
        strcat(fileName, "_statistics.txt");
    }
    strcpy(outRelPath, outDir);
    strcat(outRelPath, fileName);

    return;
}

static void printNoOfWrittenLines(int out, int no, char* fileName)
{
    char buffer[BUFSIZ];
    sprintf(buffer, "%d lines were written to %s.\n", no, fileName);
    write(out, buffer, strlen(buffer));
    return;
}

static int countLinesInFile(char* path)
{
    FILE* in = NULL;
    in = fopen(path, "r");
    if (in == NULL)
    {
        fprintf(stderr, "Could not open file %s to count lines.\n", path);
        exit(EXIT_FAILURE);
    }
    int lines = 0;
    char buffer[BUFSIZ];
    while (fgets(buffer, BUFSIZ, in) != NULL)
    {
        ++lines;
    }
    return lines - 1;
}

static int getBMPRasterDataOffset(int in)
{
    int dataOffset;
    lseek(in, RASTER_DATA_OFFSET, SEEK_SET);
    read(in, &dataOffset, 4);
    return dataOffset;
}

static void BMPToBlackAndWhite(int in, int width, int height)
{
    unsigned char bluePixel, greenPixel, redPixel;
    int opSize, i, rasterStart = getBMPRasterDataOffset(in);

    lseek(in, rasterStart, SEEK_SET);

    for (i = 0; i < (width * height); ++i)
    {
        opSize = read(in, &bluePixel, 1);
        if (!opSize)
        {
            fprintf(stderr, "Could not read blue pixel from BMP file!\n");
            exit(EXIT_FAILURE);
        }
        opSize = read(in, &greenPixel, 1);
        if (!opSize)
        {
            fprintf(stderr, "Could not read green pixel from BMP file!\n");
            exit(EXIT_FAILURE);
        }
        opSize = read(in, &redPixel, 1);
        if (!opSize)
        {
            fprintf(stderr, "Could not read red pixel from BMP file!\n");
            exit(EXIT_FAILURE);
        }
        /// Calculate grey pixel value
        unsigned char greyPixel = .114 * bluePixel + .587 * greenPixel + .299 * redPixel;
        lseek(in, -3, SEEK_CUR);
        /// Write grey pixel to all pixels
        opSize = write(in, &greyPixel, 1);
        if (!opSize)
        {
            fprintf(stderr, "Could not write grey pixel to blue pixel in BMP file!\n");
            exit(EXIT_FAILURE);
        }
        opSize = write(in, &greyPixel, 1);
        if (!opSize)
        {
            fprintf(stderr, "Could not write grey pixel to green pixel in BMP file!\n");
            exit(EXIT_FAILURE);
        }
        opSize = write(in, &greyPixel, 1);
        if (!opSize)
        {
            fprintf(stderr, "Could not write grey pixel to red pixel in BMP file!\n");
            exit(EXIT_FAILURE);
        }
    }

    return;
}

static void traverseDirectory(DIR* dir, struct dirent* dirent, char* parsedFolder, char* outputFolder, struct stat* filestat, ReqStats* reqstats)
{
    char statsFilePath[BUFSIZ];
    strcpy(statsFilePath, outputFolder);
    strcat(statsFilePath, "/");
    strcat(statsFilePath, "statistics.txt");

    int statsFile = open(statsFilePath, O_WRONLY | O_CREAT | O_TRUNC, S_IWUSR);
    if (statsFile == -1)
    {
        fprintf(stderr, "Could not open output statistics file : %s!", statsFilePath);
        exit(EXIT_FAILURE);
    }

    while ((dirent = readdir(dir)) != NULL)
    {
        char relativePath[BUFSIZ];
        strcpy(relativePath, parsedFolder);
        strcat(relativePath, "/");
        strcat(relativePath, dirent->d_name);
        /// Get stats for target
        if (lstat(relativePath, filestat) == -1)
        {
            fprintf(stderr, "Could not get stats for %s!\n", relativePath);
            exit(EXIT_FAILURE);
        }

        char outRelativePath[BUFSIZ];
        assembleDirEntFilePath(dirent->d_name, outRelativePath, outputFolder);

        int out = open(outRelativePath, O_WRONLY | O_CREAT | O_TRUNC, S_IWUSR);
        if (out == -1)
        {
            fprintf(stderr, "Could not open output file : %s!\n", outRelativePath);
            exit(EXIT_FAILURE);
        }

        int pfd[2];
        if (pipe(pfd) < 0)
        {
            fprintf(stderr, "Could not create pipe!\n");
            exit(EXIT_FAILURE);
        }

        int noOfWrittenLines, buffer, returnCode;
        pid_t pid;

        /// Determine action
        if (S_ISREG(filestat->st_mode))
        {
            int in = open(relativePath, O_RDONLY);
            getFileStats(in, dirent->d_name, reqstats);

            if ((pid = fork()) < 0)
            {
                fprintf(stderr, "Could not fork application!\n");
                exit(EXIT_FAILURE);
            }
            else if (pid == 0)
            {
                pid_t toBlackAndWhitePid;
                int returnCode;
                printFileStats(out, *reqstats);
                close(out);

                buffer = countLinesInFile(outRelativePath);
                close(pfd[0]);
                write(pfd[1], &buffer, sizeof(int));
                close(pfd[1]);

                if (strstr(relativePath, ".bmp"))
                {
                    if ((toBlackAndWhitePid = fork()) < 0)
                    {
                        fprintf(stderr, "Could not fork application!\n");
                        exit(EXIT_FAILURE);
                    }
                    if (toBlackAndWhitePid == 0)
                    {
                        BMPToBlackAndWhite(in, reqstats->lungime, reqstats->inaltime);
                        exit(EXIT_SUCCESS);
                    }
                }

                wait(&returnCode);
                fprintf(stdout, "S-a incheiat procesul cu PID : %d, cu codul %d.\n", toBlackAndWhitePid, returnCode);
                exit(EXIT_SUCCESS);
            }
            close(pfd[1]);
            read(pfd[0], &noOfWrittenLines, sizeof(int));
            close(pfd[0]);
            printNoOfWrittenLines(statsFile, noOfWrittenLines, outRelativePath);
            wait(&returnCode);
            fprintf(stdout, "S-a incheiat procesul cu PID :%d, cu codul %d.\n", pid, returnCode);
            close(in);
        }
        else if (S_ISDIR(filestat->st_mode))
        {
            if ((pid = fork()) < 0)
            {
                fprintf(stderr, "Could not fork application!\n");
                exit(EXIT_FAILURE);
            }
            if (pid == 0)
            {
                printDirStats(out, filestat, dirent->d_name);
                close(out);

                buffer = countLinesInFile(outRelativePath);
                close(pfd[0]);
                write(pfd[1], &buffer, sizeof(int));
                close(pfd[1]);
                exit(EXIT_SUCCESS);
            }
            close(pfd[1]);
            read(pfd[0], &buffer, sizeof(int));
            close(pfd[0]);
            printNoOfWrittenLines(statsFile, noOfWrittenLines, outRelativePath);
            wait(&returnCode);
            fprintf(stdout, "S-a incheiat procesul cu PID :%d, cu codul %d.\n", pid, returnCode);
        }
        else if (S_ISLNK(filestat->st_mode))
        {
            struct stat symLinkTargetStats;
            if (stat(relativePath, &symLinkTargetStats) == -1)
            {
                fprintf(stderr, "Could not get stats for %s!\n", relativePath);
                exit(EXIT_FAILURE);
            }

            if ((pid = fork()) < 0)
            {
                fprintf(stderr, "Could not fork application!\n");
                exit(EXIT_FAILURE);
            }
            if (pid == 0)
            {
                printSymLinkStats(out, filestat, dirent->d_name, *reqstats, symLinkTargetStats.st_size);
                close(out);

                buffer = countLinesInFile(outRelativePath);
                close(pfd[0]);
                write(pfd[1], &buffer, sizeof(int));
                close(pfd[1]);
                exit(EXIT_SUCCESS);
            }
            close(pfd[1]);
            read(pfd[0], &buffer, sizeof(int));
            close(pfd[0]);
            printNoOfWrittenLines(statsFile, noOfWrittenLines, outRelativePath);
            wait(&returnCode);
            fprintf(stdout, "S-a incheiat procesul cu PID :%d, cu codul %d.\n", pid, returnCode);
        }
        close(statsFile);
    }
    return;
}

int main(int argc, char **argv)
{
    /// Check args
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <input_directory> <output_directory>.\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    struct dirent* dirent = NULL;
    DIR* dir = opendir(argv[1]);
    if (dir == NULL)
    {
        fprintf(stderr, "Could not open given input directory : %s!\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    struct stat fileStats;
    ReqStats reqStats;

    traverseDirectory(dir, dirent, argv[1], argv[2], &fileStats, &reqStats);

    closedir(dir);

    return 0;
}

