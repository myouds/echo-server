#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 4

struct task {
    int client_socket;
    void (*task)(int c_sock);

    struct task *next;
};

struct task    *taskq = NULL;
pthread_mutex_t taskq_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  taskq_cond = PTHREAD_COND_INITIALIZER;
pthread_t       pool[NUM_THREADS] = {0};

struct task* task_enqueue(int client_socket, void (*task)(int c_sock))
{
    struct task *new_task,
                *q_item;

    if ((new_task = malloc(sizeof(struct task))) == NULL) {
        perror("malloc failed");
        return NULL;
    }
    new_task->client_socket = client_socket;
    new_task->task = task;
    new_task->next = NULL;

    pthread_mutex_lock(&taskq_mutex);
    if (taskq) {
        for (q_item = taskq; q_item->next; q_item = q_item->next);
        q_item->next = new_task;
    }
    else {
        taskq = new_task;
    }
    pthread_cond_signal(&taskq_cond);
    pthread_mutex_unlock(&taskq_mutex);

    return new_task;
}

struct task* task_dequeue()
{
    struct task *oldest;

    oldest = taskq;
    if (taskq) {
        taskq = taskq->next;
        oldest->next = NULL;
    }

    return oldest;
}

void* worker_loop(void *arg)
{
    struct task *job;

    pthread_mutex_lock(&taskq_mutex);
    while(1) {
        while ((job = task_dequeue()) != NULL) {
            pthread_mutex_unlock(&taskq_mutex);
            job->task(job->client_socket);
            free(job);
            pthread_mutex_lock(&taskq_mutex);
        }
        pthread_cond_wait(&taskq_cond, &taskq_mutex);
    }
}

int worker_init()
{
    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_create(&(pool[i]), NULL, &worker_loop, NULL) != 0) {
            perror("Failed to create worker thread");
            return -1;
        }
    }

    return 0;
}
