#include <unistd.h>

void create_pipe (int* fds, int num_cmds) {
    int a;
    if (num_cmds > 1) {
        for (a=0; a<(num_cmds-1); a++) {
            //printf("creating pipe fds[%d]\n", a*2);
            if (pipe(&fds[a*2]) < 0)
                perror("pipe error");
        }
    }
}

void read_pipe (int* fds) {
    if (close(fds[1]) == -1) /* Write end is unused */
        perror("error close 3");
    /* Duplicate stdin on read end of pipe; close duplicated descriptor */
    if (fds[0] != STDIN_FILENO) {
        //printf("duping fds[0] to stdin\n");
        if (dup2(fds[0], STDIN_FILENO) == -1)
            perror("error dup2 2");
        if (close(fds[0]) == -1)
            perror("error close 4");
    }
}

void write_pipe (int* fds) {
    if (close(fds[0]) == -1) /* Read end is unused */
        perror("error close 1");
    /* Duplicate stdout on write end of pipe; close duplicated descriptor */
    if (fds[1] != STDOUT_FILENO) {
        //printf("duping fds[1] to stdout\n");
        if (dup2(fds[1], STDOUT_FILENO) == -1)
            perror("error dup2 1");
        if (close(fds[1]) == -1)
            perror("error close 2");
    }
}

void close_pipes (int* fds, int num_cmds) {
    int a;
    for (a=0; a<2*(num_cmds-1); a++) {
        //printf("closing pipe fds[%d]\n", a);
        close(fds[a]); /* close pipe */
            //perror("error close 1");
    }
}
