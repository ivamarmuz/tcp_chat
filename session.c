#ifndef SESSION_H
#define SESSION_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "session.h"

struct session_list *create_session_list()
{
    struct session_list *tmp = (struct session_list *) malloc(sizeof(struct session_list));

    tmp->size = 0;
    tmp->head = tmp->tail = NULL;

    return tmp;
}

void delete_list (struct session_list **list)
{
    if (*list) {
        struct session *tmp = (*list)->head;
        struct session *next = NULL;

        while (tmp) {
            next = tmp->next;
            free(tmp);
            tmp = next;
        }

        free(*list);
        (*list) = NULL;
    }

    return;
}

void add_fd(int fd, char *ip, struct session_list *list)
{
    if (list) {
        struct session *tmp = (struct session *) malloc(sizeof(struct session));

        tmp->fd = fd;
        tmp->name = NULL;
        strcpy(tmp->ip, ip);
        tmp->next = NULL;
        tmp->prev = list->tail;

        if (list->tail)
            list->tail->next = tmp;
        
        if (list->head == NULL)
            list->head = tmp;

        list->tail = tmp;
        list->size++;
    }

    return;  
}

void delete_fd(int fd, struct session_list *list)
{
    if (list) {
        struct session *tmp = list->head;

        while (tmp && tmp->fd != fd) {
            tmp = tmp->next;
        }
        if (tmp == NULL)
            return;

        if (tmp->prev)
            tmp->prev->next = tmp->next;

        if (tmp->next)
            tmp->next->prev = tmp->prev;

        if (!tmp->prev)
            list->head = tmp->next;

        if (!tmp->next)
            list->tail = tmp->prev;

        free(tmp);
        list->size--;
    }

    return;
}

void print_list(struct session_list *list)
{
    if (list) {
        struct session *tmp = list->head;

        if (!tmp) {
            printf("=================\n");
            printf("[No clients]\n");
            return;
        }
        printf("=================\n");
        printf("Clients: %d\n", list->size);

        while (tmp) {
            printf("%s - %s\n ", tmp->name, tmp->ip);
            tmp = tmp->next;
        }

        printf("=================\n");
    }

    return;
}

void set_name(int fd, char *name, struct session_list *list)
{
    if (list) {
        struct session *tmp = list->head;

        while (tmp && tmp->fd != fd)
            tmp = tmp->next;

        tmp->name = (char *) malloc(sizeof(name));
        strcpy(tmp->name, name);
    }

    return;
}

#endif