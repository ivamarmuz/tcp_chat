struct session {
    int fd;
    struct session *first, *last, *next_fd, *prev_fd;
};

void add_fd(int, struct session *);
void delete_fd(int, struct session *);
void printList(struct session *);