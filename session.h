#ifndef SESSION_H
#define SESSION_H

#define BUFFER_SIZE 1024
#define NAME_SIZE 15

struct session {
    int fd;
    char name[NAME_SIZE];
    char buffer[BUFFER_SIZE];
    struct session *prev_session, *next_session;
};

struct session *add_fd(int, struct session *);
void delete_fd(int, struct session *);

#endif