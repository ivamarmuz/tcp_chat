#ifndef SESSION_H
#define SESSION_H

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "session.h"

void add_fd(int fd, struct session *s)
{
    printf("Start add\n");
    struct session *new_fd = malloc(sizeof(*new_fd));
    printf("BEFORE FD\n");
    new_fd->fd = fd;
    printf("BEFORE NEXT_FD\n");
    new_fd->next_fd = NULL;
    printf("BEFORE IF\n");
    if (!s->first) {
        printf("In first\n");
        new_fd->prev_fd = NULL;
        s->first = new_fd;
        s->last = s->first;

        return;
    } else {
        printf("In else\n");
        new_fd->prev_fd = s->last;
        s->last->next_fd = new_fd;
        s->last = new_fd;

        return;
    }
}

void delete_fd(int fd, struct session *s)
{
    struct session *tmp = NULL;

    if (s->first->fd == fd) {
        tmp = s->first;
        s->first = s->first->next_fd;
        if (s->first)
            s->first->prev_fd = NULL;
        free(tmp);
        return;
    }

    if (s->last->fd == fd) {
        tmp = s->last;
        s->last = s->last->prev_fd;
        if (s->last)
            s->last->next_fd = NULL;
        free(tmp);
        return;
    }
    
    tmp = s->first;

    while (tmp->fd != fd)
        tmp = tmp->next_fd;
    tmp->prev_fd->next_fd = tmp->next_fd;
    tmp->next_fd->prev_fd = tmp->prev_fd;
    free(tmp);

    return;
}

void printList(struct session *s)
{
    struct session *tmp = s->first;

    if (s->first)
        printf("First: %d\n", s->first->fd);

    if (s->last)
        printf("Last: %d\n", s->last->fd);

    while (tmp) {
        printf("%d;\n", tmp->fd);
        tmp = tmp->next_fd;
    }
}

#endif