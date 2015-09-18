/**
 *	EE 319K Project 1
 *  @file   yash.c
 *  @Author Ben Fu (byf69)
 *  @date   September 2015
 *  @brief  Simple Unix shell
*/

/* standard libs */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <malloc.h>
#include <stdbool.h>
#include <errno.h>

/* headers for UNIX */
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

/* our headers */
#include "pipe.h"
#include "job.h"

/* constants */
#define BUFSIZE 10000
#define MAXARGS 5000

/* color */
#define RESET       "\033[0m"
#define CMDCOLOR    "\033[1m\033[34m"      /* Bold Blue */

/* function definitions */
void create_process(char*, char**, char**, int, int, job*);
int split(char*, char*, char*[]);
void parse_input(char*, char**, char**, char*, int*);
int is_empty(char*);

/* signal handler function */
static void	sig_handler(int);

/* global pointers */
job *running_job = NULL;
job *foreground_job = NULL;
job *stopped_job = NULL;
job *completed_job = NULL;
job *first_job = NULL;
job *first_completed_job = NULL;

int main(int argc, char *argv[])
{
    //shell_init();
    /* The active jobs are linked into a list.  This is its head.   */
    first_job = (job*)malloc(sizeof(job));
    first_job->cmd = NULL;

    first_completed_job = (job*)malloc(sizeof(job));
    first_completed_job->cmd = NULL;

    char buf[BUFSIZE];
    pid_t pid;
    pid_t ppid; /* parent's pid */
    pid_t cpid; /* child's pid */

    if (signal(SIGINT, sig_handler) == SIG_ERR)
        perror("SIGINT error");
    if (signal(SIGTSTP, sig_handler) == SIG_ERR)
        perror("SIGTSTP error");
    if (signal(SIGCONT, sig_handler) == SIG_ERR)
        perror("SIGCONT error");
    if (signal(SIGCHLD, sig_handler) == SIG_ERR)
        perror("SIGCHLD error");

    printf(CMDCOLOR "$ " RESET);	/* print prompt $ */

    while (fgets(buf, sizeof(buf), stdin) != NULL)
    {
        /* print the jobs that just completed and remove them */
        print_jobs(first_completed_job);
        first_completed_job = remove_completed_jobs(first_completed_job);

        if (buf[strlen(buf) - 1] == '\n')
            buf[strlen(buf) - 1] = '\0'; /* replace newline with null */

        /* check for whitespace */
        if (is_empty(buf) == 1)
        {
            printf(CMDCOLOR "$ " RESET);	/* print prompt $ */
            continue;
        }

        char *parsed_cmd[MAXARGS]; /* parsed buf */
        char *usr_cmd = strdup(buf);    /* save the original cmd */

        /* parse using our parse function */
        char *cmds[MAXARGS];
        char *tokens[MAXARGS];
        char order[MAXARGS];
        int parse_res[2];   /* to contain num_cmds and num_tokens */
        parse_input(buf, cmds, tokens, order, parse_res);  /* call our parse function */
        /* interpret the results */
        int num_cmds = parse_res[0];
        int num_tokens = parse_res[1];

        if (strcmp(buf, "fg") == 0)
        {
            job *last_job = find_last_job(first_job);
            if (last_job)
            {
                printf("%s\n", last_job->cmd);
                int status;
                //printf("running most recent process in fg\n");
                kill(-1*last_job->pgid, SIGCONT);
                foreground_job = last_job;
                last_job->status = 0;
                /* wait for child process to exit */
                //printf("now waiting for children in group %d...\n", -1*last_job->pgid);
                while ((pid = waitpid(-1*last_job->pgid, &status, WUNTRACED)) > 0)
                {
                    //printf("child %d changed state\n", pid);
                    if (WIFEXITED(status))
                    {
                        //printf("child %d terminated normally\n", pid);
                        remove_process(last_job, pid);
                    }
                    if (WIFSIGNALED(status))
                    {
                        //printf("child terminated by signal\n");
                        pid_t pgid = find_terminated_pgid(pid, first_job);
                        first_job = remove_job(first_job, pgid);
                        foreground_job = NULL;
                    }
                    if (WIFSTOPPED(status))
                    {
                        //printf("child stopped\n");
                        last_job->status = 1;    /* list job as stopped */
                        stopped_job = last_job;  /* update stopped job pointer */
                        foreground_job = NULL;
                        if (WSTOPSIG(status) == SIGTSTP)
                            break;
                    }
                }
                //printf("all children exited successfully\n");
                if (job_is_completed(last_job) == 1)
                {
                    first_job = remove_job(first_job, last_job->pgid);
                }
                stopped_job = find_stopped_job(first_job);
                foreground_job = NULL;
            }
        }
        else if (strcmp(buf, "bg") == 0)
        {
            if (stopped_job)
            {
                print_last_job(first_job);
                //printf("running most recent stopped process in bg\n");
                kill(-1*stopped_job->pgid, SIGCONT);
                stopped_job->status = 0;
            }
        }
        else if (strcmp(buf,"jobs") == 0)
        {
            print_jobs(first_job);
        }
        else
        {
            /* add to job table */
            job *j = add_job(first_job, usr_cmd);
            create_process(usr_cmd, cmds, tokens, num_cmds, num_tokens, j);
        }
        printf(CMDCOLOR "$ " RESET);
    }
    exit(0);
}

void create_process(char* usr_cmd, char* cmds[], char* tokens[], int num_cmds, int num_tokens, job *cur_job)
{
    pid_t pid;
    pid_t cpid;
    pid_t ppid;
    int status;
    pid_t pgid = 0;

    /* handle backgrounding */
    bool is_background = false;
    if (num_tokens > 0 && (strcmp(tokens[num_tokens-1], "&") == 0))
    {
        //printf("background\n");
        is_background = true;
    }
    /* log job as running */
    cur_job->status = 0;
    foreground_job = cur_job;
    /* create pipes */
    int fds[MAXARGS];
    create_pipe(fds, num_cmds);
    /* setup pipe variables */
    bool is_pipe = false;
    int mode = 0;   /* 0 = write to pipe, 1 = read from pipe, 2 = both */
    int next_mode = 0;
    /* setup iterators */
    int i = 0;  /* cmds */
    int j = 0;  /* tokens */
    int k = 0;  /* index of pipe char */
    int pipe_index = 0; /* index for fds */

    /* fork n processes, n=num_cmds */
    for (i=0; i<num_cmds; i++)
    {
        //printf("entering loop\n");
        if (mode == 2)
        {
            pipe_index += 2;
        }
        /* set next mode to current mode */
        if (next_mode == 1)
        {
            //printf("just came from pipe\n");
            mode = 1;
        }
        /* check for next pipe */
        if (j < num_tokens)
        {
            for (k=j; k<num_tokens; k++)
            {
                if (strcmp(tokens[k], "|") == 0)
                    break;
            }
            /* now k should either be the index of the first pipe or num_tokens if there were no pipes */
            //printf("k is %d and j is %d\n", k, j);

            if (strcmp(tokens[j], "|") == 0)
            {
                if (mode == 0)     /* set next mode to read */
                {
                    //printf("detected pipe\n");
                    is_pipe = true;
                    next_mode = 1;
                }
                else if (mode == 1)      /* if current mode is 1 and pipe is found, change mode to 2 */
                {
                    mode = 2;
                    next_mode = 1;
                }
            }
        }

        /* fork a child */
        pid = fork();

        switch (pid)
        {

        /* fork error */
        case -1:
            perror("fork error");
            exit(1);
            break;

        /* child */
        case 0:
            cpid = getpid();
            //printf("I am child with pid %d\n", cpid);
            /* special case if child gets here before parent */
            if (i == 0)     /* set pgid to the first child's pgid */
            {
                cur_job->pgid = cpid;
                if (setpgid(cpid, 0) == -1)
                {
                    perror("setpgid");
                }
                else
                {
                    if ((pgid = getpgid(cpid)) == -1)
                    {
                        perror("getpgid");
                    }
                    //printf("setting pgid of %d to %d\n", cpid, pgid);
                }
            }
            else
            {
                /* make pgid of current process to the job's pgid */
                pgid = cur_job->pgid;
                //printf("current pgid of job is %d\n", pgid);
                if (setpgid(cpid, pgid) == -1)
                {
                    perror("setpgid");
                }
                else
                {
                    //printf("setting pgid of %d to %d\n", cpid, pgid);
                }
            }


            if (is_pipe)
            {
                /* open/close pipe based on mode */
                if (mode == 0)      /* write to pipe */
                {
                    //printf("writing to pipe fds[%d]\n", pipe_index);
                    write_pipe(&fds[pipe_index]);
                }
                else if (mode == 1)      /* read */
                {
                    //printf("reading from pipe fds[%d]\n", pipe_index);
                    read_pipe(&fds[pipe_index]);
                }
                else    /* read from pipe and write to next pipe */
                {
                    //printf("reading from pipe fds[%d] and writing to pipe fds[%d]\n", pipe_index, pipe_index+2);
                    read_pipe(&fds[pipe_index]);
                    write_pipe(&fds[pipe_index+2]);
                }
                /* close all pipes */
                close_pipes(&fds[0], num_cmds);
            }
            else
            {
                //printf("no pipes\n");
            }

            if (j < num_tokens)
            {
                int l = j;
                int m = i;
                /* redirect inputs/outputs as needed */
                for (l=j; l<k; l++)
                {
                    if (strcmp(tokens[j], ">") == 0)
                    {
                        /* dup file to stdout */
                        //printf("redirecting output\n");
                        int fd_out = open(cmds[m+1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
                        if (dup2(fd_out, STDOUT_FILENO) == -1)
                            perror("dup2 error");
                        close(fd_out);
                        m++;
                        j++;
                    }
                    else if (strcmp(tokens[j], "<") == 0)
                    {
                        /* dup file to stdin */
                        //printf("redirecting input\n");
                        int fd_in = open(cmds[m+1], O_RDONLY, S_IRUSR | S_IWUSR);
                        if (dup2(fd_in, STDIN_FILENO) == -1)
                            perror("dup2 error");
                        close(fd_in);
                        m++;
                        j++;
                    }
                    else if (strcmp(tokens[j], "2>") == 0)
                    {
                        /* dup file to stdin */
                        //printf("redirecting stderr\n");
                        int fd_err = open(cmds[m+1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
                        if (dup2(fd_err, STDERR_FILENO) == -1)
                            perror("dup2 error");
                        close(fd_err);
                        m++;
                        j++;
                    }
                    else if (strcmp(tokens[j], "2>&1") == 0)
                    {
                        /* dup file to stdin */
                        //printf("redirecting stderr to stdout\n");
                        if (dup2(STDOUT_FILENO, STDERR_FILENO) == -1)
                            perror("dup2 error");
                        m++;
                        j++;
                    }

                }
            }
            /* Set the handling for job control signals back to the default.  */
            signal (SIGINT, SIG_DFL);
            signal (SIGQUIT, SIG_DFL);
            signal (SIGTSTP, SIG_DFL);
            signal (SIGTTIN, SIG_DFL);
            signal (SIGTTOU, SIG_DFL);
            signal (SIGCHLD, SIG_DFL);

            char *exec_args[MAXARGS];   /* array of pointers to hold the args */
            char* tmp_cmd = strdup(cmds[i]);

            int count = split(cmds[i], " ", exec_args);
            exec_args[count] = NULL;    /* set args[i] to NULL for exec */

            /* execute the program */
            //printf("now executing command %s...\n", tmp_cmd);
            execvpe(exec_args[0], (char *)exec_args);

            /* if exec failed to exit, then there was an error */
            printf("%s: command not found\n", usr_cmd);
            exit(127);
            break;

        /* parent */
        default:
            if (i == 0)     /* set pgid to the first child's pgid */
            {
                if (setpgid(pid, 0) == -1)
                {
                    perror("setpgid");
                }
                //printf("setting pgid to %d\n", pid);
                cur_job->pgid = pid;
                running_job = cur_job;
            }
            /* add process to process list */
            add_process(cur_job, pid);
        }
        j = k + 1;  /* process has handled everything up to pipe or EOL */

    }

    ppid = getpid();
    /* print parent pid */
    //printf("I am parent with pid %d\n", ppid);

    if (is_pipe)
    {
        /* close all pipes */
        close_pipes(&fds[0], num_cmds);
    }

    if (!is_background)
    {
        /* wait for child process to exit */
        //printf("now waiting for children in group %d...\n", -1*cur_job->pgid);
        while ((pid = waitpid(-1*cur_job->pgid, &status, WUNTRACED)) > 0)
        {
            //printf("child %d changed state\n", pid);
            if (WIFEXITED(status))
            {
                //printf("child terminated normally\n");
                remove_process(cur_job, pid);
            }
            if (WIFSIGNALED(status))
            {
                //printf("child terminated by signal\n");
                pgid = find_terminated_pgid(pid, first_job);
                first_job = remove_job(first_job, pgid);
            }
            if (WIFSTOPPED(status))
            {
                //printf("child stopped\n");
                cur_job->status = 1;    /* list job as stopped */
                stopped_job = cur_job;  /* update stopped job pointer */
                if (WSTOPSIG(status) == SIGTSTP)
                    break;
            }
        }
        if (job_is_completed(cur_job) == 1)     /* extra check that all jobs have finished */
        {
            first_job = remove_job(first_job, cur_job->pgid);
        }
        stopped_job = find_stopped_job(first_job);
        foreground_job = NULL;
    }
    else    /* background */
    {
        foreground_job = NULL;
    }
}

int split(char *string, char *delim, char *tokens[])
{
    int count = 0;
    char *token;
    char *stringp;

    stringp = string;
    while (stringp != NULL)
    {
        token = strsep(&stringp, delim);
        tokens[count] = token;
        count++;
    }
    return count;
}
/* parse the input */
void parse_input(char *buf, char *cmds[], char *tokens[], char *order, int *result)
{
    int i = 0;  /* buf index */
    int j = 0;  /* cmds index */
    int k = 0;  /* tokens index */
    int l = 0;  /* order index */
    int cmd_start = 0;  /* command start index */
    bool append_cmd = true; /* default yes to append command at end of loop, exceptions are & and 2>&1 */

    int num_pipes = 0;   /* num_cmds will be num_pipes-1 */

    while (buf[i] != 0)
    {
        int offset;
        char token = buf[i];

        switch (token)
        {
        case '>':
        case '<':
        case '|':   /* single tokens */
            if (buf[i] == '|')
                num_pipes++;
            buf[i-1] = '\0';    /* change the space before token to end of string */
            buf[i+1] = '\0';    /* change the space after token to end of string */
            tokens[k] = &buf[i]; /* copy token into  tokens array */
            cmds[j] = &buf[cmd_start];   /* copy string into cmds array */
            order[l] = 'c';
            order[l+1] = 't';   /* log the order of cmd then token */

            /* increment indexes */
            i += 1; /* increment i past the null we inserted at i+1 */
            cmd_start = i + 1;  /* next word will start 2 positions after token */
            j++;
            k++;
            l += 2;
            break;

        case '2':   /* multiple character tokens */
            if (buf[i+1] != '>')    /* just a character, not a token */
            {
                break;
            }
            /* either 2>&1 or 2> */
            else if (buf[i+1] ==  '>' && buf[i+2] == '&')    /* has to be 2>&1 token */
            {
                /* there's another token after 2>&1 */
                buf[i-1] = '\0';    /* change the space before token to end of string */
                buf[i+4] = '\0';    /* change the space after token to end of string */
                tokens[k] = &buf[i];    /* copy token into  tokens array */
                cmds[j] = &buf[cmd_start];   /* copy string into cmds array */
                order[l] = 'c';
                order[l+1] = 't';   /* log the order of command, token, and then token */

                /* increment indexes */
                cmd_start = i + 2;  /* pretend the &1 is the start of the next command */
                i += 4; /* increment i to the next cmd/arg */
                j++;
                k++;
                l += 2;
                break;
            }
            else   /* has to be 2> token */
            {
                buf[i-1] = '\0';    /* change the space before token to end of string */
                buf[i+2] = '\0';    /* change the space after token to end of string */
                tokens[k] = &buf[i]; /* copy token into  tokens array */
                cmds[j] = &buf[cmd_start];   /* copy string into cmds array */
                order[l] = 'c';
                order[l+1] = 't';   /* log the order of cmd then token */

                /* increment indexes */
                i += 2; /* increment i past the null we inserted at i+1 */
                cmd_start = i + 1;  /* next word will start 2 positions after token */
                j++;
                k++;
                l += 2;
            }
            break;

        case '&':
            if (buf[i+1] == '\0')   /* last token of line */
            {
                buf[i-1] = '\0';    /* change the space before token to end of string */
                tokens[k] = &buf[i];    /* copy token into  tokens array */
                cmds[j] = &buf[cmd_start];   /* copy string into cmds array */
                order[l] = 'c';
                order[l+1] = 't';   /* log the order of cmd then token */
                append_cmd = false; /* don't append command since token was last thing typed */

                /* increment indexes */
                i ++; /* increment i to the EOL so while loop will exit */
                j++;
                k++;
                l += 2;
            }
            else    /* just the &1 part of 2>&1, treat it like a normal command */
            {

            }
            break;
        default:
            break;
        }
        i++;
    }

    /* add the extra command for the one after the last token (not accounted for in while loop) */
    if (append_cmd)
    {
        cmds[j] = &buf[cmd_start];
        order[l] = 'c';
        j++;
        l++;
    }
    /* make sure ends of arrays are null */
    cmds[j] = NULL;
    tokens[k] = NULL;
    order[l] = '\0';
    /* write to result */
    //printf("total of %d pipes\n", num_pipes);
    result[0] = num_pipes + 1;
    result[1] = k;
}
/* check if line is empty */
int is_empty(char *buf)
{
    char *b = buf;
    while (*b != '\0')
    {
        if (!isspace(*b))
            return 0;
        b++;
    }
    return 1;
}

void sig_handler(int signo)
{
    pid_t pid;
    pid_t pgid;
    int status;
    //printf("signal caught for process %d\n", pid);
    switch (signo)
    {
    case SIGINT:
        //printf("sending SIGINT\n");
        if (foreground_job)
            kill(-1*running_job->pgid, SIGINT);
        break;
    case SIGTSTP:
        //printf("sending SIGTSTP to %d\n", running_job->pgid);
        if (foreground_job)
            kill(-1*running_job->pgid, SIGTSTP);
        break;
    case SIGCONT:
        //printf("continuing job");
        if (stopped_job)
            kill(-1*(stopped_job->pgid), SIGCONT);
        break;
    case SIGCHLD:
        //printf("SIGCHLD\n");
        while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
        {
            //printf("child %d changed state\n", pid);
            if (WIFEXITED(status))  /* log child as finished and print */
            {
                job *j;
                process *p;
                pgid = find_terminated_pgid(pid, first_job);
                //printf("pgid of terminated child is %d\n", pgid);
                j = find_job(pgid, first_job);
                p = find_process(pid, j->first_process);
                p->status = 2;  /* log status as completed! */
                if (job_is_completed(j) == 1)
                {
                    j->status = 2;  /* completed job */
                    remove_all_processes(j);    /* clean up */
                    completed_job = j;
                    add_completed_job(first_completed_job, completed_job);
                    first_job = remove_job(first_job, pgid);
                    stopped_job = find_stopped_job(first_job);
                    //printf("yay!\n");
                }
            }
            if (WIFSIGNALED(status))
            {
                running_job->status = 2;
            }
            if (WIFSTOPPED(status))
            {
                stopped_job->status = 1;    /* list process as stopped */
                if (WSTOPSIG(status) == SIGTSTP)
                    break;
            }
            /* Handle the death of pid p */
        }
        break;

    default:
        //printf("matches none of the signals\n");
        break;
    }
}
