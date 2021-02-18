#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include "session.h"

#define PORT 7890
#define BUFFER_SIZE 1023

int main(void)
{
    int ls, fd, max_fd, res, tmp_del;
    char buffer[BUFFER_SIZE];
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
                fd = accept(ls, (struct sockaddr *)&client, &slen);
                add_fd(fd, fd_list);
                if (max_fd < fd)
                    max_fd = fd;
                printf("New client\n");
            }
        }

        tmp = fd_list->head;

        while (tmp) {
            memset(buffer, 0, BUFFER_SIZE);

            if (FD_ISSET(tmp->fd, &rds)) {
                if ((read(tmp->fd, buffer, BUFFER_SIZE)) != 0) {
                    printf(" > %s", buffer);
                } else {
                    if (tmp->fd == max_fd) {
                        tmp2 = fd_list->head;
                        max_fd = ls;
                        while(tmp2) {
                            if (tmp2->fd > max_fd && tmp2->fd != tmp->fd)
                                max_fd = tmp2->fd;
                            tmp2 = tmp2->next;
                        }
                        tmp_del = tmp->fd;
                        close(tmp->fd);
                        delete_fd(tmp_del, fd_list);
                        printf("Connection closed\n");
                    }
                }
            }

            tmp = tmp->next;
            print_list(fd_list);
        }
    }

    return 0;
}