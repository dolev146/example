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
#include "list.h"

char *gid_to_name(gid_t uid);
char modestr[11];
char *uid_to_name(uid_t uid);
char *gid_to_name(gid_t gid);
char *mod_to_letters(int mode, char str[]);
int dir = 0;
int counter = 0;
int files = 0;
int num = 0;
char *strr;

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
            {
                counter++;
                printf("%s\n", strr); // print the first . directory
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
            printf("%s\n", &pathname[ftwb->base]); // print the first . directory
            counter++;
            return 0;
        }
        node_t *new_node = (node_t *)malloc(sizeof(node_t));

        if (ftwb->level == 1)
        {
            new_node->data_before = (nodebefore_t *)malloc(sizeof(nodebefore_t));
            new_node->data_before->data = (char *)malloc(sizeof(char) * strlen("├── "));
            strcpy(new_node->data_before->data, "├── ");
            counter++;
            printf("├── ");
        }

        else
        {
            counter++;
            new_node->data_before = (nodebefore_t *)malloc(sizeof(char) * 3);
            int i = 0;
            while (i < ftwb->level - 1)
            {
                printf("│  ");
                new_node->data_before = (nodebefore_t *)malloc(sizeof(nodebefore_t));
                new_node->data_before->data = (char *)malloc(sizeof(char) * strlen("│  "));
                strcpy(new_node->data_before->data, "│  ");
                i++;
            }
            printf(" └── ");
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
            printf(" %s \n", &pathname[ftwb->base]); /* Print basename */
            dir++;
        }
        else
        {
            int spaceNumber = ftwb->level * 3;

            if (greenflag == 1)
            {

                printf(" %s   %s               ", uid_to_name(info.st_uid), gid_to_name(info.st_uid));

                while (spaceNumber > 0)
                {
                    printf(" ");
                    spaceNumber--;
                }
                printf(" %ld] ", info.st_size);

                printf(" %s \n", &pathname[ftwb->base]); /* Print basename */
            }
            else

            {

                printf(" %s   %s               ", uid_to_name(info.st_uid), gid_to_name(info.st_uid));
                while (spaceNumber > 0)
                {
                    printf(" ");
                    spaceNumber--;
                }
                printf(" %ld] ", info.st_size);

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
    {
        printf("\n%d directories, %d files\n", dir, files);
    }

    printf(" finish \n \n");

    print_all_nodes();

    exit(EXIT_SUCCESS);
}

char *mod_to_letters(int mode, char str[])
{
    static char perms[11];
    perms[0] = S_ISDIR(mode) ? 'd' : '-';
    perms[1] = mode & S_IRUSR ? 'r' : '-';
    perms[2] = mode & S_IWUSR ? 'w' : '-';
    perms[3] = mode & S_IXUSR ? 'x' : '-';
    perms[4] = mode & S_IRGRP ? 'r' : '-';
    perms[5] = mode & S_IWGRP ? 'w' : '-';
    perms[6] = mode & S_IXGRP ? 'x' : '-';
    perms[7] = mode & S_IROTH ? 'r' : '-';
    perms[8] = mode & S_IWOTH ? 'w' : '-';
    perms[9] = mode & S_IXOTH ? 'x' : '-';
    perms[10] = '\0';
    strcpy(str, perms);
    return perms;
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
