/*
** EPITECH PROJECT, 2023
** B-NWP-400-MAR-4-1-myteams-florian.grima [WSL : Ubuntu]
** File description:
** load_users
*/

#include "server.h"
#include <stdio.h>

void free_userlist(char ***userList)
{
    for (int i = 0; userList[i]; i++) {
        for (int j = 0; userList[i][j]; j++) {
            free(userList[i][j]);
        }
        free(userList[i]);
    }
    free(userList);
}

void load_users_from_save(void)
{
    DIR *d; struct dirent *dir; char *path = NULL;
    char ***userList = malloc(sizeof(char **) * 100);
    memset(userList, 0, 100);
    int index = 0; d = opendir("users/");
    if (!d)
        return;
    while ((dir = readdir(d)) != NULL) {
        path = dir->d_name;
        if (strlen(path) > 4 && !strcmp(path + strlen(path) - 4, ".txt")) {
            int size = strlen("users/") + strlen(dir->d_name) + 1;
            char *fullpath = malloc(sizeof(char ) * size);
            memset(fullpath, '\0', size);
            strcpy(fullpath, "users/");
            strcat(fullpath, dir->d_name);
            char **val = read_user_from_save(fullpath);
            free(fullpath); userList[index] = val;
            index++;
        }
    }
    send_user_loaded(userList); closedir(d); free_userlist(userList);
}

void send_user_loaded(char ***userList)
{
    fd_set read_fds; FD_ZERO(&read_fds); FD_SET(STDERR_FILENO, &read_fds);
    for (int i = 0; userList[i] ; i++) {
        select(STDERR_FILENO + 1, NULL, &read_fds, NULL, NULL);
        server_event_user_loaded(userList[i][1], userList[i][0]);
    }
}

char **read_user_from_save(char *path)
{
    FILE *fptr = fopen(path, "r");
    if (!fptr)
        return NULL;
    fd_set read_fds;FD_ZERO(&read_fds);FD_SET(fileno(fptr), &read_fds);
    select(fileno(fptr) + 1, &read_fds, NULL, NULL, NULL);
    size_t len = 0;
    char *line = NULL;
    char **array = malloc(sizeof(char * ) * 100);
    memset(array, 0, 100);
    int i = 0;
    while (getline(&line, &len, fptr) != -1) {
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }
        array[i] = line;
        line = NULL;
        i++;
        select(fileno(fptr) + 1, &read_fds, NULL, NULL, NULL);
    }
    fclose(fptr); return array;
}
