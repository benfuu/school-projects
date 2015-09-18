#include <unistd.h>
/* useful structs */
/* A process is a single process.  */
typedef struct process
{
    struct process *next;       /* next process in pipeline */
    pid_t pid;                  /* process ID */
    int status;                 /* 0 for running, 1 for stopped, 2 for completed */
} process;
/* A job is a pipeline of processes.  */
typedef struct job
{
    struct job *next;             /* next active job */
    pid_t pgid;                   /* process group ID */
    char *cmd;                    /* command line, used for messages */
    int status;                   /* 0 for running, 1 for stopped, 2 for completed */
    process *first_process;       /* list of all process id's that are part of the job */
    int num_processes;            /* true if user told about stopped job */
} job;
/* process fucntions */
void add_process(job*, pid_t);
void remove_process(job*, pid_t);
void remove_all_processes(job*);
process* find_process(pid_t, process*);
/* job functions */
job* add_job(job*, char*);
job* remove_job(job*, pid_t);
void add_completed_job(job*, job*);
job* remove_completed_jobs(job*);
job* find_job(pid_t, job*);
job* find_stopped_job(job*);
job* find_last_job(job*);
int job_is_stopped(job*);
int job_is_completed(job*);
void print_jobs(job* first_job);
void print_last_job(job* first_job);
pid_t find_terminated_pgid(pid_t, job*);
