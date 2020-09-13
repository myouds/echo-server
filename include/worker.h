#ifndef __WORKER_H__
#define __WORKER_H__

int worker_init();
struct task* task_enqueue(int client_socket, void (*task)(int c_sock));

#endif
