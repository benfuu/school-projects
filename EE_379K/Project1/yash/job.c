#include <malloc.h>
#include <string.h>
#include <sys/types.h>
#include "job.h"

/* adds process to process table in current job */
void add_process (job *j, pid_t pid)
{
    process *p = (process*)malloc(sizeof(process));
    p->next = NULL;
    p->pid = pid;
    p->status = 0;  /* process is running when added */
    process *first_process = j->first_process;
    if (first_process == NULL) {    /* no first process, so assign */
        j->first_process = p;
        //printf("first process added!\n");
        return;
    }
    else {  /* there's already a first process */
        process *k = first_process;
        while (k->next != NULL) { /* iterate until first_process is empty */
            k = k->next;
        }
        k->next = p;
        //printf("process added to list!\n");
    }

}
/* removes process from process group */
void remove_process (job *j, pid_t pid)
{
    process *first_process = j->first_process;
    if (first_process == NULL) {    /* no processes....wtf? */
        //printf("no processes to delete...\n");
    }
    else if (first_process->next == NULL) { /* this is our only process */
        free(first_process);
        j->first_process = NULL;
        //printf("this is the only process. removing...\n");
    }
    /* more than one process in process list */
    else if (first_process->pid == pid) {   /* remove first process */
        j->first_process = first_process->next;
        free(first_process);
        //printf("this was the first process in the list. removing...\n");
    }
    else {  /* iterate through process list */
        process *k = first_process;
        process *tmp = k;
        while (k->next != NULL){    /* iterate until k->next is null */
            tmp = k;    /* tmp will hold the second to last element (which we want) */
            k = k->next;
            if (k->pid == pid) {   /* found the process! */
                if (k->next != NULL) {  /* link previous job to next job */
                    tmp->next = k->next;
                }
                else {  /* delete the reference */
                    tmp->next = NULL;
                }
                /* free the current pointer and set its reference to null */
                free(k);
                //printf("removed process!\n");
            }
        }
        //printf("couldn't find pid...uh oh\n");
    }
}
/* removes all processes from a job */
void remove_all_processes(job *j)
{
    if (j->first_process == NULL) {
        //printf("no processes to remove\n");
        return;
    }
    else {
        process *first_process = j->first_process;
        process *k = first_process;
        process *tmp = k;   /* to save k once k is removed */
        if (k->next == NULL) {  /* this is the only process, so remove */
            remove_process(j, k->pid);
            return;
        }
        while (k != NULL) {   /* remove all the processes */
            tmp = k;
            remove_process(j, k->pid);
            k = tmp->next;
        }
        //printf("all processes removed!");
    }
}
/* finds process within a process chain */
process* find_process(pid_t pid, process *first_process)
{
    process *p = first_process;
    if (p->pid == pid) {
        //printf("process found!\n");
        return p;
    }
    while (p != NULL) {
        if (p->pid == pid) {
            //printf("process found!\n");
            return p;
        }
        p = p->next;
    }
    //printf("pid not found\n");
    return NULL;
}
/* will add a job to the job table */
job* add_job (job *first_job, char *cmd)
{
    /* allocate space for a job */
    job *j = (job*)malloc(sizeof(job));
    j->next = NULL;
    j->cmd = cmd;
    j->pgid = 0;    /* user will assign pgid */
    j->first_process = NULL;
    j->num_processes = 0;
    /* no first job, so set first job to j */
    if (first_job->cmd == NULL) {
        //printf("creating first job\n");
        first_job->cmd = j->cmd;
        first_job->pgid = j->pgid;
        first_job->next = j->next;
        first_job->first_process = j->first_process;
        first_job->num_processes = j->num_processes;
        free(j);    /* we don't need j anymore, so free it */
        j = first_job;
    }
    else {  /* there's already a job */
        job *k = first_job;
        while (k->next != NULL){    /* iterate until k->next is null */
            k = k->next;
        }
        k->next = j;
    }
    return j;
}

/* removes the job with specified pid, returns the new root of the list */
job* remove_job(job *first_job, pid_t pid)
{
    if (first_job->cmd == NULL) {
        //printf("nothing to remove\n");
    }
    else if (first_job->next == NULL) { /* this is our only job */
        first_job->cmd = NULL;  /* adjust cmd so print_jobs will not print it */
        //printf("is only job... removed!\n");
    }
    /* more than one job in queue */
    else if (first_job->pgid == pid) {  /* first job matches pid */
        printf("matches\n");
        /* there's more than one job in the list, so delete this node */
        job *tmp = first_job->next;
        free(first_job);
        first_job = tmp;
        //printf("setting first job to next job %s", first_job->cmd);
    }
    else {  /* iterate through the job list */
        //printf("iterating...\n");
        job *k = first_job;
        job *tmp = k;
        while (k != NULL){    /* iterate until k->next is null */
            if (k->pgid == pid) {
                if (k->next != NULL) {  /* link previous job to next job */
                    tmp->next = k->next;
                }
                else {
                    tmp->next = NULL;
                }
                /* free the current pointer and set its reference to null */
                free(k);
                //printf("removed!\n");
            }
            tmp = k;    /* tmp will hold the second to last element (which we want) */
            k = k->next;
        }
        //printf("couldn't find pid...uh oh\n");
    }
    return first_job;
}
/* will add a job to the completed job table */
void add_completed_job (job *first_job, job *new_job)
{
    /* allocate space for a job */
    job *j = (job*)malloc(sizeof(job));
    j->next = NULL;
    j->cmd = new_job->cmd;
    j->pgid = new_job->pgid;
    j->status = new_job->status;
    /* no first job, so set first job to j */
    if (first_job->cmd == NULL) {
        //printf("setting first job\n");
        first_job->cmd = j->cmd;
        first_job->pgid = j->pgid;
        first_job->next = j->next;
        first_job->status = j->status;
        free(j);    /* we don't need j anymore, so free it */
        j = first_job;
    }
    else {  /* there's already a job */
        job *k = first_job;
        while (k->next != NULL){    /* iterate until k->next is null */
            k = k->next;
        }
        k->next = j;
    }
    return;
}
job* remove_completed_jobs(job *first_job)
{
    if (first_job->cmd == NULL) {
        //printf("nothing to remove\n");
    }
    else if (first_job->next == NULL) { /* this is our only job */
        first_job->cmd = NULL;  /* adjust cmd so print_jobs will not print it */
        //printf("is only job... removed!\n");
    }
    else {  /* remove all jobs */
        first_job->next = NULL;
        first_job->cmd = NULL;
        first_job->first_process = NULL;
        first_job->pgid = 0;
        job *k = first_job;
        job *tmp;
        while (k != NULL){    /* iterate until k->next is null */
            tmp = k->next;    /* save k */
            /* free the current pointer and set its reference to null */
            if (k != first_job) {
                free(k);
                //printf("removed!\n");
            }
            k = tmp;
        }
        //printf("couldn't find pid...uh oh\n");
    }
    return first_job;
}
/* finds job based on the pid given, returns the job if found */
job* find_job (pid_t pgid, job *first_job)
{
    job *j = first_job;
    if (j->pgid == pgid) {
        return j;
    }
    while (j != NULL && j->cmd != NULL) {
        if (j->pgid == pgid)
            return j;
        j = j->next;
    }
    //printf("job not found\n");
    return NULL;
}
/* finds last job that is stopped */
job* find_stopped_job(job *first_job)
{
    job *j = first_job;
    job *stopped = NULL;
    while (j != NULL) {   /* iterate until j has no next */
        if (j->status == 1) {   /* save index of stopped job */
            stopped = j;
        }
        j = j->next;
    }
    return stopped;
}
/* finds last job that is EITHER stopped or running */
job* find_last_job(job *first_job)
{
    job *j = first_job;
    job *last = j;
    while (j != NULL) {   /* iterate until j has no next */
        last = j;   /* save last job */
        j = j->next;
    }
    if (last->cmd == NULL) {
        return NULL;
    }
    return last;
}

/* Return true if all processes in the job have stopped or completed.  */
int job_is_stopped (job *j)
{
    return 1;
}
/* Return true if all processes in the job have completed.  */
int job_is_completed (job *j)
{
    if (j->first_process == NULL) { /* if there's no processes, then return as done */
        //printf("job finished!\n");
        return 1;
    }
    else {  /* there's at least one process */
        process *p = j->first_process;
        while (p != NULL) {
            if (p->status != 2) {   /* if process is not completed, then return 0 */
                return 0;
            }
            p = p->next;
        }
        //printf("job is completed\n");
        return 1;   /* if all processes have finished, return true */
    }
}
/* prints all the jobs from the job table including status */
void print_jobs (job *first_job) {
    job *j = first_job;
    int i = 1;  /* job index */

    while (j != NULL && j->cmd != NULL) { /* print out all jobs and descriptions */
        printf("[%d]", i);
        if (j->next == NULL) {  /* j is last job */
            printf("+ ");
        }
        else {  /* j is not the last job */
            printf("- ");
        }
        if (j->status == 0) {
            printf("Running");
        }
        else if (j->status == 1) {
            printf("Stopped");
        }
        else {
            printf("Done");
        }
        printf("\t%s\n", j->cmd);
        i++;
        j = j->next;
    }
}
/* prints the last job in the table */
void print_last_job (job *first_job)
{
    job *j = first_job;
    job *tmp = j;
    int i = 0;  /* job index */
    printf("%s\n", tmp->cmd);

    while (j != NULL && j->cmd != NULL) { /* print out all jobs and descriptions */
        i++;
        tmp = j;
        j = j->next;
    }
    tmp->cmd = strcat(tmp->cmd, " &");
    printf("[%d]+ %s\n", i, tmp->cmd);
}
/* finds the pgid of a terminated child process */
pid_t find_terminated_pgid (pid_t pid, job *first_job)
{
    job *j = first_job;
    process *p = j->first_process;
    while (j != NULL) {
        if ((p = find_process(pid, j->first_process)) != NULL) {  /* found! */
            //printf("found pgid!\n");
            return j->pgid;
        }
        j = j->next;
    }
    //printf("can't find pgid\n");
    return 0;
}
