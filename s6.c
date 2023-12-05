#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

typedef struct {
    char numeFisier[BUFSIZ], drepturiUser[BUFSIZ], drepturiGrup[BUFSIZ], drepturiAltii[BUFSIZ];
    int dimensiune, inaltime, lungime, userId, nrLegaturi;
    time_t timpUltimaModificare;
} ReqStats;

static void getBMPFileStats(int in, char* filename, ReqStats* reqStats)
{
    struct stat inStat;
    int buffer;
    /// Copy filename
    strcpy(reqStats->numeFisier, filename);
    /// Dimensiune
    lseek(in, 18, SEEK_SET);
    read(in, &buffer, 4);
    reqStats->dimensiune = buffer;
    /// Lungime
    read(in, &buffer, 4);
    reqStats->lungime = buffer;
    /// Inaltime
    read(in, &buffer, 4);
    reqStats->inaltime = buffer;
    /// Get other stats with fstat
    fstat(in, &inStat);
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

static int printfBMPFileStats(int out, ReqStats stats)
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
    /// Lungime
    sprintf(buffer, "Lungime: %d.\n", stats.lungime);
    write(out, buffer, strlen(buffer));
    strcpy(buffer, "");
    /// Inaltime
    sprintf(buffer, "Inaltime: %d.\n", stats.inaltime);
    write(out, buffer, strlen(buffer));
    strcpy(buffer, "");
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

    return 0;
}

int main(int argc, char **argv)
{
    /// Check args
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <input_file>.\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    if (strstr(argv[1], ".bmp") == NULL)
    {
        fprintf(stderr, "Given argument does not point to a .bmp file : %s.\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    fprintf(stdout, "Given file path : %s.\n", argv[1]);

    /// Open required files
    /// Input bmp file
    int input_fd = open(argv[1], O_RDONLY);
    if (input_fd == -1)
    {
        fprintf(stderr, "Could not open input file.\n");
        exit(EXIT_FAILURE);
    }
    /// Output txt file
    int output_fd = open("statistica.txt", O_WRONLY);
    if (output_fd == -1)
    {
        fprintf(stderr, "Could not open output file.\n");
        exit(EXIT_FAILURE);
    }

    /// Get stats
    ReqStats reqStats;
    getBMPFileStats(input_fd, argv[1], &reqStats);
    printfBMPFileStats(output_fd, reqStats);

    close(input_fd);
    close(output_fd);

    return 0;
}
