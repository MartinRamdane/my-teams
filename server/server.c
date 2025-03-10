/*
** EPITECH PROJECT, 2023
** server.c
** File description:
** server
*/

#include "server.h"

struct list_head head;

void free_all_clients(void)
{
    while (!LIST_EMPTY(&head)) {
        struct client *elem = LIST_FIRST(&head);
        LIST_REMOVE(elem, next);
        free(elem->buffer);
        free(elem);
    }
}

void remove_closed_clients(void)
{
    struct client *tmp;
    LIST_FOREACH(tmp, &head, next) {
        if (tmp->sock == -1) {
            LIST_REMOVE(tmp, next);
            free(tmp->buffer);
            free(tmp);
            return;
        }
    }
}

void create_server(char *port)
{
    load_users_from_save();
    struct client *tmp = NULL; LIST_INIT(&head);
    int max_sd, master_socket = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(master_socket, 1, 2, &(int){1}, sizeof(int));
    fd_set readfds;
    struct sockaddr_in myaddr; int addrlen = sizeof(myaddr);
    myaddr.sin_family = AF_INET; inet_aton("0.0.0.0", &myaddr.sin_addr);
    myaddr.sin_port = htons(atoi(port));
    bind(master_socket, (struct sockaddr *)&myaddr, sizeof(myaddr));
    listen(master_socket, MAX_CLIENTS);
    while (1) {
        add_and_set_sockets(&readfds, &max_sd, master_socket);
        int ret_val = select(max_sd + 1, &readfds, NULL, NULL, NULL);
        if (ret_val <= 0)
            return unload_users_from_save();
        if (FD_ISSET(master_socket, &readfds))
            accept_socket(master_socket, myaddr, addrlen);
        operations_on_sockets(&readfds, tmp); remove_closed_clients();
    }
    close(master_socket); free_all_clients();
}
