#define _XOPEN_SOURCE 600 
#include <ftw.h>
#include <sys/types.h> 
#include <stdio.h>     
#include <stdlib.h>    
#include <unistd.h>    
#include <errno.h>     
#include <string.h>    
#include <sys/stat.h>
#include <pwd.h>
#include <string.h>
#include <dirent.h>
#include <grp.h>

// Define the ANSI color codes
#define COLOR_GREEN "\x1b[32m"
#define COLOR_BLUE "\x1b[34m"
#define COLOR_RESET "\x1b[0m"

char *gid_to_name(gid_t uid);
char modestr[11];
char *uid_to_name(uid_t uid);
char *gid_to_name(gid_t gid);
void mod_to_letters(int mode, char str[]);
int dir = 0;
int counter = 0;
int files = 0;
int num = 0;
char *strr;

// printf("└── ");

static int /* Callback function called by ftw() */
dirTree(const char *pathname, const struct stat *sbuf, int type, struct FTW *ftwb)
{

    struct stat info;
    if (type != FTW_NS && stat(pathname, &info) != -1)
    {
        if (num > 1)
        {
            mod_to_letters(info.st_mode, modestr);

            if (strstr(strr, "/") != NULL && counter == 0 && modestr[0] == 'd')
            { /// handle the path/path argument
                counter++;
                printf("%s\n", strr);
                return 0;
            }
            if (modestr[0] != 'd' && counter == 0)
            {
                printf("%s [error opening dir]\n", strr);
                counter++;
                return 0;
            }
        }
        if ((strcmp(pathname + (ftwb->base), ".") == 0) || (strcmp(pathname + (ftwb->base), pathname) == 0))
        {
            printf(COLOR_BLUE "%s\n" COLOR_RESET, &pathname[ftwb->base]);
            counter++;

            return 0;
        }
        if (ftwb->level == 1)
        {
            counter++;
            printf("├──");
        }

        else
        {
            counter++;
            // printf("│  ");
            int i = 0;
            while (i < ftwb->level - 1)
            {
                printf("│  ");
                i++;
            }
            printf("└──");
        }

        mod_to_letters(info.st_mode, modestr);
        // check if the modestr contains 'x' and if it does, print it in green
        int greenflag = 0;
        for (int i = 0; i < 10; i++)
        {
            if (modestr[i] == 'x')
            {
                greenflag = 1;
                break;
            }
        }

        printf("[%s", modestr);

        if (modestr[0] == 'd')
        {
            // directory
            printf(" %s   %s               %ld]   ", uid_to_name(info.st_uid), gid_to_name(info.st_uid), info.st_size);
            printf(COLOR_BLUE " %s \n" COLOR_RESET, &pathname[ftwb->base]); /* Print basename */
            dir++;
        }
        else
        {
            if (greenflag == 1)
            {
                printf(" %s   %s               %ld]   ", uid_to_name(info.st_uid), gid_to_name(info.st_uid), info.st_size);
                printf(COLOR_GREEN " %s \n" COLOR_RESET, &pathname[ftwb->base]); /* Print basename */
            }
            else

            {

                printf(" %s    %s               %ld]   ", uid_to_name(info.st_uid), gid_to_name(info.st_uid), info.st_size);
                printf(" %s \n", &pathname[ftwb->base]); /* Print basename */
            }
            files++;
        }
    }

    else
    {
        printf("---");
    }

    return 0;
}

int main(int argc, char *argv[])
{
    int flags = 0;
    char *path = ".";
    if (argc == 1)
    {
        num = argc;
        if (nftw(path, dirTree, 10, flags) == -1)
        {
            fprintf(stderr, "Usage: %s directory-path\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        num = argc;
        for (int i = 1; i < argc; i++)
        {
            strr = argv[i];
            if (nftw(argv[i], dirTree, 10, flags) == -1)
            {
                printf("%s [error opening dir]\n", strr);
            }
        }
    }
    if (argc)

        printf("\n%d directories, %d files\n", dir, files);

    exit(EXIT_SUCCESS);
}

void mod_to_letters(int mode, char str[])
{
    strcpy(str, "----------");
    if (S_ISDIR(mode))
        str[0] = 'd';
    if (S_ISCHR(mode))
        str[0] = 'c';
    if (S_ISBLK(mode))
        str[0] = 'b';
    if (mode & S_IRUSR)
        str[1] = 'r';
    if (mode & S_IWUSR)
        str[2] = 'w';
    if (mode & S_IXUSR)
        str[3] = 'x';

    if (mode & S_IRGRP)
        str[4] = 'r';
    if (mode & S_IWGRP)
        str[5] = 'w';
    if (mode & S_IXGRP)
        str[6] = 'x';

    if (mode & S_IROTH)
        str[7] = 'r';
    if (mode & S_IWOTH)
        str[8] = 'w';
    if (mode & S_IXOTH)
        str[9] = 'x';
}
char *uid_to_name(uid_t uid)
{
    return getpwuid(uid)->pw_name;
}
char *gid_to_name(gid_t gid)
{
    struct group *getgrgid(), *grp_ptr;
    static char numstr[10];
    if ((grp_ptr = getgrgid(gid)) == NULL)
    {
        sprintf(numstr, "%d", gid);
        return numstr;
    }
    else
    {
        return grp_ptr->gr_name;
    }
}
