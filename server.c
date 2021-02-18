#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#include "session.h"

#define PORT 7890
#define BUFFER_SIZE 1023
#define IP_SIZE 18

int main(void)
{
    int ls, fd, max_fd, res, last_msg;
    char buffer[BUFFER_SIZE], ip[IP_SIZE];
    struct session_list *fd_list = create_session_list();
    struct session *tmp = fd_list->head, *tmp2  = NULL;
    struct sockaddr_in server, client;
    socklen_t slen = sizeof(struct sockaddr_in);
    fd_set rds, wrs;

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;

    ls = socket(AF_INET, SOCK_STREAM, 0);

    if (ls == -1)
        perror("Socket error");

    if ((bind(ls, (struct sockaddr *)&server, sizeof(server))) == -1)
        perror("Bind error");

    max_fd = ls;
    listen(ls, 5);
    printf("Listening on port %d...\n", PORT);

    for(;;) {
        FD_ZERO(&rds);
        FD_ZERO(&wrs);
        FD_SET(ls, &rds);

        tmp = fd_list->head;

        while(tmp) {
            FD_SET(tmp->fd, &rds);

            tmp = tmp->next;
        }

        res = select(max_fd+1, &rds, NULL, NULL, NULL);

        if (res <= 0) {
            if (errno == EINTR)
                continue;
            else
                break;
        }

        if (res > 0) {
            if (FD_ISSET(ls, &rds)) {
                memset(ip, 0, IP_SIZE);
                fd = accept(ls, (struct sockaddr *)&client, &slen);
                strcpy(ip, inet_ntoa(client.sin_addr));
                add_fd(fd, ip, fd_list);
                FD_SET(fd, &wrs);
                
                if (max_fd < fd)
                    max_fd = fd;
            }
        }

        tmp = fd_list->head;

        while (tmp) {
            if (FD_ISSET(tmp->fd, &rds)) {
                memset(buffer, 0, BUFFER_SIZE);
                last_msg = tmp->fd;
                if ((read(tmp->fd, buffer, BUFFER_SIZE)) != 0) {
                    if (!tmp->name) {
                        set_name(tmp->fd, buffer, fd_list);
                        memset(buffer, 0, BUFFER_SIZE);
                        tmp2 = fd_list->head;
                        strncat(buffer, tmp->name, sizeof(tmp->name));
                        strncat(buffer, " connected\n", 12);
                        while (tmp2) {
                            if (tmp2->fd != last_msg) {
                                write(tmp2->fd, buffer, sizeof(buffer));
                            }
                            tmp2 = tmp2->next;
                        }
                        memset(buffer, 0, BUFFER_SIZE);
                        printf("[%s connected]\n", tmp->name);
                        strncat(buffer, "Welcome to chat, ", 18);
                        strncat(buffer, tmp->name, sizeof(tmp->name));
                        write(tmp->fd, buffer, sizeof(buffer));
                        memset(buffer, 0, BUFFER_SIZE);
                        print_list(fd_list);
                    } else {
                        printf("%s: %s", tmp->name, buffer);
                        tmp2 = fd_list->head;
                        strncat(buffer, ": ", 3);
                        strncat(buffer, tmp->name, sizeof(tmp->name));
                        while (tmp2) {
                            if (tmp2->fd != last_msg) {
                                write(tmp2->fd, buffer, sizeof(buffer));
                            }
                            tmp2 = tmp2->next;
                        }
                        memset(buffer, 0, BUFFER_SIZE);
                    }
                } else {
                    if (tmp->fd == max_fd) {
                        tmp2 = fd_list->head;
                        max_fd = ls;
                        while(tmp2) {
                            if (tmp2->fd > max_fd && tmp2->fd != tmp->fd)
                                max_fd = tmp2->fd;
                            tmp2 = tmp2->next;
                        }
                    }
                    close(tmp->fd);
                    memset(buffer, 0, BUFFER_SIZE);
                    tmp2 = fd_list->head;
                    strncat(buffer, tmp->name, sizeof(tmp->name));
                    strncat(buffer, " disconnected\n", 15);
                    while (tmp2) {
                        if (tmp2->fd != last_msg) {
                            write(tmp2->fd, buffer, sizeof(buffer));
                        }
                        tmp2 = tmp2->next;
                    }
                    memset(buffer, 0, BUFFER_SIZE);
                    printf("[%s disconnected]\n", tmp->name);
                    delete_fd(tmp->fd, fd_list);
                    print_list(fd_list);
                    tmp = tmp->next;
                    continue;
                }
            }

            if (FD_ISSET(tmp->fd, &wrs)) {
                if (!tmp->name)
                    write(tmp->fd, "Enter your name: ", 18);
            }

            tmp = tmp->next;
        }
    }

    return 0;
}