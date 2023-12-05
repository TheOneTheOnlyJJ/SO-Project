#include <dirent.h>
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

static void getFileStats(int in, char* filename, ReqStats* reqStats)
{
    struct stat inStat;
    int buffer;
    /// Copy filename
    strcpy(reqStats->numeFisier, filename);
    /// Dimensiune
    lseek(in, 18, SEEK_SET);
    read(in, &buffer, 4);
    reqStats->dimensiune = buffer;
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

static void traverseDirectory(int* out, DIR* dir, struct dirent* dirent, char* parsedFolder, struct stat* filestat, ReqStats* reqstats)
{
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
        /// Determine action
        if (S_ISREG(filestat->st_mode))
        {
            int in = open(relativePath, O_RDONLY);
            getFileStats(in, dirent->d_name, reqstats);
            printFileStats(*out, *reqstats);
            close(in);
        }
        else if (S_ISDIR(filestat->st_mode))
        {
            printDirStats(*out, filestat, dirent->d_name);
        }
        else if (S_ISLNK(filestat->st_mode))
        {
            struct stat symLinkTargetStats;
            if (stat(relativePath, &symLinkTargetStats) == -1)
            {
                fprintf(stderr, "Could not get stats for %s!\n", relativePath);
                exit(EXIT_FAILURE);
            }
            printSymLinkStats(*out, filestat, dirent->d_name, *reqstats, symLinkTargetStats.st_size);
        }
    }
    return;
}

int main(int argc, char **argv)
{
    /// Check args
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <input_directory>.\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    /// Open required files
    /// Input directory
    DIR* input_dir = NULL;
    input_dir = opendir(argv[1]);
    if (input_dir == NULL)
    {
        fprintf(stderr, "Could not open given directory!\n");
        exit(EXIT_FAILURE);
    }
    /// Output txt file
    int output_fd = open("statistica.txt", O_WRONLY);
    if (output_fd == -1)
    {
        fprintf(stderr, "Could not open output file!\n");
        exit(EXIT_FAILURE);
    }
    struct stat filestat;
    ReqStats reqstats;
    struct dirent* dirent = NULL;

    traverseDirectory(&output_fd, input_dir, dirent, argv[1], &filestat, &reqstats);

    closedir(input_dir);
    close(output_fd);

    return 0;
}
