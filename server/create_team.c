/*
** EPITECH PROJECT, 2023
** create_team.c
** File description:
** create_team
*/

#include "server.h"

void write_new_member(char *u_uuid, char *t_uuid)
{
    int size = strlen("teams/") + strlen(t_uuid) + 6;
    char *path = malloc(sizeof(char) * size);
    memset(path, '\0', size);
    strcpy(path, "teams/"); strcat(path, t_uuid); strcat(path, ".txt");
    int i = find_last_line(path);
    set_file_line(i, t_uuid, u_uuid, "teams/");
    free(path); size = strlen("users/") + strlen(t_uuid) + 6;
    path = malloc(sizeof(char) * size); memset(path, '\0', size);
    strcpy(path, "users/"); strcat(path, u_uuid); strcat(path, ".txt");
    i = find_last_line(path);
    set_file_line(i, u_uuid, t_uuid, "users/"); free(path);
}

void send_reply_code_team(struct client *client, char *id, char *name,
    char *desc)
{
    send(client->sock, "910\n", 4, 0);
    struct client *tmp;
    LIST_FOREACH(tmp, &head, next) {
        if (tmp->username != NULL && tmp->id != NULL) {
            send(tmp->sock, "911 ", 4, 0);
            send(tmp->sock, "\"", 1, 0);
            send(tmp->sock, id, 36, 0);
            send(tmp->sock, "\" \"", 3, 0);
            send(tmp->sock, name, strlen(name), 0);
            send(tmp->sock, "\" \"", 3, 0);
            send(tmp->sock, desc, strlen(desc), 0);
            send(tmp->sock, "\"\n", 2, 0);
        }
    }
}

void create_team_command(struct client *client, char *buffer)
{
    char *team_name, *team_uuid, *team_desc;
    char **file = malloc(sizeof(char *) * 100); memset(file, 0, 100);
    buffer += 12; buffer[strlen(buffer)] = '\0';
    char *token = strtok(buffer, " ");
    team_uuid = token; token = strtok(NULL, "\""); team_name = token;
    strtok(NULL, "\""); token = strtok(NULL, "\""); team_desc = token;
    if (check_if_title_exist(team_name, "./teams/") == 1) {
        send(client->sock, "312\n", 4, 0); return;
    }
    server_event_team_created(team_uuid, team_name, client->id);
    create_team_file(team_uuid, team_name, team_desc);
    send_reply_code_team(client, team_uuid, team_name, team_desc);
}

void create_team_file(char *t_uuid, char *t_name, char *t_desc)
{
    int size = strlen("teams/") + strlen(t_uuid) + 6;
    char *path = malloc(sizeof(char) * size);
    memset(path, '\0', size);
    strcpy(path, "teams/");
    strcat(path, t_uuid);
    strcat(path, ".txt");
    FILE *fd = fopen(path, "w");
    fwrite(t_uuid, 1, strlen(t_uuid), fd);
    fwrite("\n", 1, 1, fd);
    fwrite(t_name, 1, strlen(t_name), fd);
    fwrite("\n", 1, 1, fd);
    fwrite(t_desc, 1, strlen(t_desc), fd);
    fwrite("\n1\n0\n", 1, 5, fd);
    fclose (fd);
    free(path);
}
