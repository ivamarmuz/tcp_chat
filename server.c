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
    int ls, fd, max_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t slen = sizeof(struct sockaddr_in);
    char buffer[BUFFER_SIZE];
    struct session *fd_list = NULL, *tmp = fd_list, *tmp2 = NULL;
    fd_set rds, wrs;
    int flags;

    fd_list = malloc(sizeof(*fd_list));
    fd_list->first = NULL;
    fd_list->last = NULL;

    ls = socket(AF_INET, SOCK_STREAM, 0);

    if (ls == -1)
        perror("Socket error");

    max_fd = ls;

    flags = fcntl(ls, F_GETFL);
    fcntl(ls, F_SETFL, flags | O_NONBLOCK);

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if ((bind(ls, (struct sockaddr *)&server_addr, sizeof(server_addr))) == -1)
        perror("Bind error");

    listen(ls, 5);
    printf("Listening on port 7890...\n");

    for (;;) {
        FD_ZERO(&rds);
        FD_ZERO(&wrs);
        FD_SET(ls, &rds);
        
        tmp = fd_list->first;

        while (tmp) {
            FD_SET(tmp->fd, &rds);
            tmp = tmp->next_fd;
        }

        int res = select(max_fd+1, &rds, NULL, NULL, NULL);

        if (res < 0) {
            if (errno == EINTR)
                continue;
            else
                break;
        }

        if (res > 0) {

            if (FD_ISSET(ls, &rds)) {
                fd = accept(ls, (struct sockaddr* )&client_addr, &slen);
                add_fd(fd, fd_list);
                if (max_fd < fd)
                    max_fd = fd;
                printf("New connection\n");
            }

            tmp = fd_list->first;

            while (tmp) {
                memset(buffer, 0, BUFFER_SIZE);

                if (FD_ISSET(tmp->fd, &rds)) {
                    if ((read(tmp->fd, buffer, BUFFER_SIZE)) == 0) {
                        if (tmp->fd == max_fd) {
                            tmp2 = fd_list->first;
                            while (tmp2) {
                                if (max_fd < tmp2->fd)
                                    max_fd = tmp2->fd;
                                tmp2 = tmp2->next_fd;
                            }
                        }
                        close(tmp->fd);
                        delete_fd(tmp->fd, fd_list);
                        tmp->fd = -1;
                        printf("Connection closed\n");
                    } else {
                        printf(" > %s", buffer);
                    }
                }
                tmp = tmp->next_fd;
            }
        }
    }
    
    return 0;
}