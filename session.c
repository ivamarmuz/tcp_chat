#include "session.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct session *add_fd(int fd, struct session *first)
{
    struct session *tmp = malloc(sizeof(*tmp));

    tmp->fd = fd;
    memset(tmp->name, 0, NAME_SIZE);
    memset(tmp->buffer, 0, BUFFER_SIZE);
    tmp->prev_session = NULL;
    tmp->next_session = first;
    if (first) {
        first->prev_session = tmp;
    }
    return tmp;
}

void delete_fd(int fd, struct session *first)
{
    struct session *tmp = first;
    if (fd == tmp->fd) {
        if (tmp->prev_session) {
            tmp->prev_session->next_session = tmp->next_session;
        }
        if (tmp->next_session) {
            tmp->next_session->prev_session = tmp->prev_session;
        }
        if (!tmp->prev_session && !tmp->next_session) {
            first = NULL;
        }
        tmp->fd = -1;
        free(tmp);
    } else {
        delete_fd(fd, tmp->next_session);
    }
}