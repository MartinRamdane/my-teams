/*
** EPITECH PROJECT, 2023
** B-NWP-400-MAR-4-1-myteams-florian.grima [WSL: Ubuntu]
** File description:
** handle_commands
*/

#include "client.h"

void free_client(client_t *client)
{
    free(client->username);
    close(client->sock);
    if (client->context >= 1 && client->team_uuid != NULL)
        free(client->team_uuid);
    if (client->context >= 2 && client->channel_uuid != NULL)
        free(client->channel_uuid);
    if (client->context >= 3 && client->thread_uuid != NULL)
        free(client->thread_uuid);
    free(client->s_team); free(client->s_channel); free(client->s_thread);
    free(client->s_reply); free(client->buffer);
    exit(0);
}

void handle_received_more_2(client_t *client)
{
    if (strstr(client->buffer, "931")) {
        event_thread_created(client->buffer); return;
    } if (strstr(client->buffer, "941")) {
        event_reply_created(client->buffer); return;
    } if (strstr(client->buffer, "101")) {
        client_error_unauthorized(); return;
    } if (strstr(client->buffer, "901")) {
        client_print_subscribed(client->id, client->s_team->t_uuid); return;
    } if (strstr(client->buffer, "user_logged_in: ")) {
        get_client_login_event(client); return;
    } if (strstr(client->buffer, "user_logged_out: ")) {
        get_client_logout_event(client); return;
    } if (strstr(client->buffer, "102")) {
        client_error_unauthorized(); return;
    } if (strstr(client->buffer, "902")) {
        client_print_unsubscribed(client->id, client->s_team->t_uuid); return;
    }
}

void handle_received_more(client_t *client)
{
    if (client->buffer[0] == '3' && client->buffer[2] == '2') {
        client_error_already_exist(); return;
    } if (strstr(client->buffer, "911")) {
        client_event_team_created(client->s_team->t_uuid
        , client->s_team->t_name, client->s_team->t_desc); return;
    } if (strstr(client->buffer, "921")) {
        event_channel_created(client->buffer); return;
    } if (strstr(client->buffer, "311")) {
        client_error_unknown_team(client->team_uuid); return;
    } if (strstr(client->buffer, "313")) {
        client_error_unknown_team(client->s_team->t_uuid); return;
    } if (strstr(client->buffer, "321")) {
        client_error_unknown_channel(client->channel_uuid); return;
    } if (strstr(client->buffer, "331")) {
        client_error_unknown_thread(client->s_thread->t_uuid);
    }
    handle_received_more_2(client);
}

void handle_received_data(client_t *client)
{
    if (strstr(client->buffer, "receive:")) {
        receive_message(client->buffer); return;
    } if (strstr(client->buffer, "users:")) {
        receive_users(client->buffer); return;
    } if (strstr(client->buffer, "user:")) {
        receive_user(client->buffer); return;
    } if (strstr(client->buffer, "msg_history:")) {
        receive_messages_history(client->buffer, client); return;
    } if (strstr(client->buffer, "LOGIN OK\n")) {
        client_event_logged_in(client->id, client->username);client->login = 1;
    } if (strstr(client->buffer, "LOGOUT OK\n")) {
        client_event_logged_out(client->id, client->username);
        free_client(client);
    } if (strstr(client->buffer, "/info")) {
        info_command_receive(client, client->buffer); return;
    } if (strstr(client->buffer, "/list")) {
        list_command_receive(client, client->buffer); return;
    }
    handle_received_more(client);
}
