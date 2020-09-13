#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>
#include <err.h>

#include "echo.h"
#include "worker.h"

#define SERVER_PORT 5050

int listen_on_port(int port)
{
    int                s_sock = 0;
    struct sockaddr_in serv_addr;

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(SERVER_PORT);

    if ((s_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Failed to create socket");
        return -1;
    }
    if (bind(s_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) {
        err(0, "Failed to bind to port %u", SERVER_PORT);
        return -1;
    }
    if (listen(s_sock, 10)) {
        perror("Failed to listen on IP socket");
        return -1;
    }
    return s_sock;
}

int service_clients(int s_sock, void (*task)(int client_sock))
{
    struct sockaddr_in clnt_addr;
    socklen_t          addr_len;
    int                c_sock = 0,
                       ret = 0;
    char               ipaddr_str[INET_ADDRSTRLEN];

    while(1) {
        bzero(&clnt_addr, sizeof(clnt_addr));
        addr_len = sizeof(clnt_addr);
        if ((c_sock = accept(s_sock, (struct sockaddr *)&clnt_addr, &addr_len)) < 0) {
            perror("Accept failed");
            ret = -1;
            break;
        }
        inet_ntop(AF_INET, &(clnt_addr.sin_addr.s_addr), ipaddr_str, INET_ADDRSTRLEN);
        printf("Accepted connection from %s (%u)\n", ipaddr_str, ntohs(clnt_addr.sin_port));

        task_enqueue(c_sock, task);
    }
    return ret;
}

int start_server()
{
    int ret = 0,
        s_sock = 0;

    if ((s_sock = listen_on_port(SERVER_PORT)) <= 0) {
        perror("Failed to listen on echo port");
        return -1;
    }
    if (service_clients(s_sock, &echo) != 0) {
        perror("Echo server failed");
        return -1;
    }
    return ret;
}

int main(int argc, char **argv)
{
    int code = 0;
    if (worker_init() == 0) {
        if ((code = start_server()) != 0) {
            fprintf(stderr, "Echo server failed\n");
        }
    }
    else {
        fprintf(stderr, "Failed to initialize workers\n");
        code = 1;
    }
    return code;
}
