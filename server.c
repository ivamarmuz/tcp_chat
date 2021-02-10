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

int main(int argc, char **argv)
{
    int ls, fd, max_fd, res, port = PORT;
    char buffer[BUFFER_SIZE];
    int last_message_fd;
    fd_set readfds, writefds;
    socklen_t slen;
    struct sockaddr_in server_addr, client_addr;
    struct session *fd_list = NULL, *tmp = NULL;

    if (argc > 1) {
        port = atoi(argv[1]);
    }

    ls = socket(AF_INET, SOCK_STREAM, 0);

    if (ls == -1) {
        perror("Socket error");
    }

    max_fd = ls;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if ((bind(ls, (struct sockaddr *)&server_addr, sizeof(server_addr))) == -1) {
        perror("Bind error");
    }

    fcntl(ls, O_NONBLOCK);
    listen(ls, 5);
    printf("Server has been started on port %d...\n", port);

    for (;;) {
        FD_ZERO(&readfds);
        FD_ZERO(&writefds);
        FD_SET(ls, &readfds);

        tmp = fd_list;

        while(tmp) {
            FD_SET(tmp->fd, &readfds);
            if (tmp->fd > max_fd) {
                max_fd = tmp->fd;
            }
            FD_SET(tmp->fd, &writefds);
            tmp = tmp->next_session;
        }
        
        res = select(max_fd+1, &readfds, NULL, NULL, NULL);

        if (res < 1) {
            if (errno != EINTR) {
                perror("Select error");
            } else {
                perror("Signal error");
            }
            continue;
        }

        if (res == 0) {
            printf("Timeout");
            continue;
        }

        if (FD_ISSET(ls, &readfds)) {
            fd = accept(ls, (struct sockaddr *)&client_addr, &slen);
            fd_list = add_fd(fd, fd_list);
            printf("New client\n");
        }

        tmp = fd_list;

        while(tmp) {
            if (FD_ISSET(tmp->fd, &readfds)) {
                memset(buffer, 0, BUFFER_SIZE);

                if ((read(tmp->fd, buffer, BUFFER_SIZE)) == 0) {
                    close(tmp->fd);
                    delete_fd(tmp->fd, fd_list);
                    printf("Connection closed\n");
                } else {
                    printf(" > %s", buffer);
                    last_message_fd = tmp->fd;
                }
            }
            if (FD_ISSET(tmp->fd, &writefds)) {
                if (last_message_fd != tmp->fd) {
                    write(tmp->fd, buffer, BUFFER_SIZE);
                }
            }
            tmp = tmp->next_session;
        }
    }

    return 0;
}