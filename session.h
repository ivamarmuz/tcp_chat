struct session {
    int fd;
    struct session *prev, *next;
};

struct session_list {
    int size;
    struct session *head, *tail;
};

struct session_list *create_session_list(void);
void delete_list (struct session_list **);
void add_fd(int, struct session_list *);
void delete_fd(int, struct session_list *);
void print_list(struct session_list *);