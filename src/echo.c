#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "echo.h"

#define BUFLEN      256

void echo(int client_socket) {
    char read_buff[BUFLEN];
    int  num_read;

    while((num_read = read(client_socket, read_buff, sizeof(read_buff))) > 0) {
        write(client_socket, read_buff, sizeof(read_buff[0]) * num_read);
    }
    if (close(client_socket) != 0) {
        perror("Unable to close connection");
    }
}
