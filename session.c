#ifndef SESSION_H
#define SESSION_H

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "session.h"

void add_fd(int fd, struct session *s)
{
    struct session *new_fd = malloc(sizeof(*new_fd));

    new_fd->fd = fd;
    new_fd->next_fd = NULL;

    if (!s->first) {
        new_fd->prev_fd = NULL;
        s->first = new_fd;
        s->last = s->first;

        return;
    } else {
        new_fd->prev_fd = s->last;
        s->last->next_fd = new_fd;
        s->last = new_fd;

        return;
    }
}

void delete_fd(int fd, struct session *s)
{
    
}

#endif